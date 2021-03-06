function fred_report_info() {
  var info={report_cat    : "SYSTEM",
            report_name   : "USB storage devices",
            report_author : "Gillen Daniel, Voncken Guy",
            report_desc   : "Dump USB storage devices",
            fred_api      : 2,
            hive          : "SYSTEM"
  };
  return info;
}

var table_style = "border-collapse:collapse; margin-left:20px; font-family:arial; font-size:12";
var cell_style  = "border:1px solid #888888; padding:5; white-space:nowrap;";

function IsValid(val) {
  return (typeof val!=='undefined');
}

function PrintTableHeaderCell(str) {
  println("        <th style=\"",cell_style,"\">",str,"</th>");
}

function PrintTableDataCell(alignment,str) {
  var style=cell_style+" text-align:"+alignment+";";
  println("        <td style=\"",style,"\">",str,"</td>");
}

function PrintTableDataRowSpanCell(alignment,rows,str) {
  var style=cell_style+" text-align: "+alignment+";";
  println("        <td rowspan=\"",rows,"\" style=\"",style,"\">",str,"</td>");
}

function ZeroPad(number,padlen) {
  var ret=number.toString(10);
  if(!padlen || ret.length>=padlen) return ret;
  return Math.pow(10,padlen-ret.length).toString().slice(1)+ret;
}

function GetKeyVal(path, key) {
   var val=GetRegistryKeyValue(path, key);
   return (IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "";
}

function fred_report_html() {
  // TODO: There is more here.
  // Check http://www.forensicswiki.org/wiki/USB_History_Viewing
  var val;

  println("  <h2>USB storage devices</h2>");

  // Preload MountedDevices to possibly identify mount points of USB storage
  // devices
  var mnt_keys=GetRegistryKeys("\\MountedDevices");
  var mnt_values=new Array();
  if(IsValid(mnt_keys)) {
    for(var i=0;i<mnt_keys.length;i++) {
      val=GetRegistryKeyValue("\\MountedDevices",mnt_keys[i]);
      mnt_values[i]=RegistryKeyValueToVariant(val.value,"utf16");
    }
  }

  // Get current controlset
  var cur_controlset=GetRegistryKeyValue("\\Select","Current");
  if(IsValid(cur_controlset)) {
    cur_controlset=RegistryKeyValueToString(cur_controlset.value,
                                            cur_controlset.type);
    // Current holds a DWORD value, thus we get a string like 0x00000000, but
    // control sets are referenced by its decimal representation.
    cur_controlset="ControlSet"+
                     ZeroPad(parseInt(String(cur_controlset).substr(2,8),16),3);

    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <table style=\""+table_style+"\">");

    // Are USB storage devices enabled?
    // http://www.forensicmag.com/article/windows-7-registry-forensics-part-5
    // Is this true for WinXP etc.. ???
    var val=GetRegistryKeyValue(cur_controlset+"\\services\\USBSTOR","Start");
    if(IsValid(val)) {
      val=RegistryKeyValueToString(val.value,val.type);
      val=parseInt(String(val).substr(2,8),10);
      switch(val) {
        case 3:
          println("      <tr><td>Storage driver enabled:</td><td>Yes</td></tr>");
          break;
        case 4:
          println("      <tr><td>Storage driver enabled:</td><td>No</td></tr>");
          break;
        default:
          println("      <tr><td>Storage driver enabled:</td><td>Unknown</td></tr>");
      }
    } else {
      println("      <tr><td>Storage driver enabled:</td><td>Unknown</td></tr>");
    }

    println("    </table>");
    println("  </p>");
    println("  <p style=\"font-size:12; white-space:nowrap\">");

    var storage_roots=GetRegistryNodes(cur_controlset+"\\Enum\\USBSTOR");
    if(IsValid(storage_roots)) {
      println("    <table style=\""+table_style+"\">");
      println("      <tr>");
      PrintTableHeaderCell("Vendor Name");
      PrintTableHeaderCell("Unique ID");
      PrintTableHeaderCell("Class");
      PrintTableHeaderCell("Friendly name");
      PrintTableHeaderCell("Mount point(s)");
      PrintTableHeaderCell("Parent ID");
      PrintTableHeaderCell("Device description");
      PrintTableHeaderCell("First connection<font color=\"red\"><sup>1</sup></font>");
      PrintTableHeaderCell("Last connection<font color=\"red\"><sup>1</sup></font>");
      println("      </tr>");

      for(var i=0;i<storage_roots.length;i++) {
        var storage_subroots=GetRegistryNodes(cur_controlset+"\\Enum\\USBSTOR\\"+storage_roots[i]);
        for(var ii=0;ii<storage_subroots.length;ii++) {
          var device_id=storage_subroots[ii];
          if(String(device_id).charAt(1)=="&") {
            // If the second character of the unique instance ID is a '&', then
            // the ID was generated by the system, as the device did not have a
            // serial number.
            device_id=device_id+" (Generated by system)";
          }
          var device_key=cur_controlset+"\\Enum\\USBSTOR\\"+storage_roots[i]+"\\"+storage_subroots[ii];
          var device_class=GetKeyVal(device_key,"Class");
          var device_desc=GetKeyVal(device_key,"DeviceDesc");
          var device_friendly_name=GetKeyVal(device_key,"FriendlyName");
          var device_parent_id=GetKeyVal(device_key,"ParentIdPrefix");
          var device_first_connection=GetRegistryNodeModTime(cur_controlset+"\\Enum\\USBSTOR\\"+storage_roots[i]);
          var device_last_connection=GetRegistryNodeModTime(cur_controlset+"\\Enum\\USBSTOR\\"+storage_roots[i]+"\\"+storage_subroots[ii]);

          var search_string="";
          var device_mount_points=Array();
          if(device_parent_id != "") {
            // Windows XP uses the ParentId to link to MountedDevices
            search_string="#"+device_parent_id+"&";
          } else {
            // Since Vista, Unique IDs are used
            search_string="#"+storage_subroots[ii]+"#";
          }
          for(var iii=0; iii<mnt_keys.length; iii++) {
            if(String(mnt_values[iii]).indexOf(search_string)!=-1) {
              device_mount_points.push(mnt_keys[iii]);
            }
          }

          var mount_points=device_mount_points.length;
          if(mount_points>1) {
            println("      <tr>");
            PrintTableDataRowSpanCell("left",mount_points,storage_roots[i]);
            PrintTableDataRowSpanCell("left",mount_points,device_id);
            PrintTableDataRowSpanCell("left",mount_points,device_class);
            PrintTableDataRowSpanCell("left",mount_points,device_friendly_name);
            PrintTableDataCell("left",device_mount_points[0]);
            PrintTableDataRowSpanCell("left",mount_points,device_parent_id);
            PrintTableDataRowSpanCell("left",mount_points,device_desc);
            PrintTableDataRowSpanCell("left",mount_points,device_first_connection);
            PrintTableDataRowSpanCell("left",mount_points,device_last_connection);
            println("      </tr>");
            for(var iii=1;iii<device_mount_points.length;iii++) {
              println("      <tr>");
              PrintTableDataCell("left",device_mount_points[iii]);
              println("      </tr>");
            }
          } else {
            println("      <tr>");
            PrintTableDataCell("left",storage_roots[i]);
            PrintTableDataCell("left",device_id);
            PrintTableDataCell("left",device_class);
            PrintTableDataCell("left",device_friendly_name);
            if(mount_points!=0) {
              PrintTableDataCell("left",device_mount_points[0]);
            } else {
              PrintTableDataCell("left","n/a");
            }
            PrintTableDataCell("left",device_parent_id);
            PrintTableDataCell("left",device_desc);
            PrintTableDataCell("left",device_first_connection);
            PrintTableDataCell("left",device_last_connection);
            println("      </tr>");
          }
        }
      }
      println("    </table>");
      println("    &nbsp;&nbsp;&nbsp;&nbsp;<font color=\"red\"><sup>1</sup></font> Might be incorrect");
      println("    <br />");
    } else {
      println("  <font color=\"red\">This registry hive does not contain a list of attached USB storage devices!</font>");
    }
    println("  </p>");
  } else {
    println("  <p><font color=\"red\">");
    println("    Unable to determine current control set!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }
}

