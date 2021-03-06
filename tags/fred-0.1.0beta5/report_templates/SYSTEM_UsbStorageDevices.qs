function fred_report_info() {
  var info={report_cat    : "SYSTEM",
            report_name   : "USB storage devices",
            report_author : "Gillen Daniel",
            report_desc   : "Dump USB storage devices",
            fred_api      : 2,
            hive          : "SYSTEM"
  };
  return info;
}

function IsValid(val) {
  if(typeof val !== 'undefined') return true;
  else return false;
}

function print_table_row(cell01,cell02) {
  println("      <tr><td>",cell01,"</td><td>",cell02,"</td></tr>");
}

function ZeroPad(number,padlen) {
  var ret=number.toString(10);
  if(!padlen || ret.length>=padlen) return ret;
  return Math.pow(10,padlen-ret.length).toString().slice(1)+ret;
}

function fred_report_html() {
  // TODO: There is more here. Check http://www.forensicswiki.org/wiki/USB_History_Viewing
  var val;

  println("  <h2>USB storage devices</h2>");

  // Preload MountedDevices to possibly identify mount points of USB storage devices
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
    cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
    // Current holds a DWORD value, thus we get a string like 0x00000000, but
    // control sets are referenced by its decimal representation.
    cur_controlset="ControlSet"+ZeroPad(parseInt(String(cur_controlset).substr(2,8),16),3)

    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <u>Settings</u><br />");
    println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

    // Are USB storage devices enabled?
    // http://www.forensicmag.com/article/windows-7-registry-forensics-part-5
    // Is this true for WinXP etc.. ???
    var val=GetRegistryKeyValue(cur_controlset+"\\services\\USBSTOR","Start");
    if(IsValid(val)) {
      val=RegistryKeyValueToString(val.value,val.type);
      val=parseInt(String(val).substr(2,8),10);
      switch(val) {
        case 3:
          print_table_row("Storage driver enabled:","Yes");
          break;
        case 4:
          print_table_row("Storage driver enabled:","No");
          break;
        default:
          print_table_row("Storage driver enabled:","Unknown");
      }
    } else {
      print_table_row("Storage driver enabled:","Unknown");
    }

    println("    </table>");
    println("  </p>");
    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <u>Devices</u><br />");

    var storage_roots=GetRegistryNodes(cur_controlset+"\\Enum\\USBSTOR");
    if(IsValid(storage_roots)) {
      for(var i=0;i<storage_roots.length;i++) {
        println("    <u style=\"margin-left:20px; font-size:12; white-space:nowrap\">",storage_roots[i],"</u><br />");
        var storage_subroots=GetRegistryNodes(cur_controlset+"\\Enum\\USBSTOR\\"+storage_roots[i]);
        for(ii=0;ii<storage_subroots.length;ii++) {
          println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");
          // If the second character of the unique instance ID is a '&', then the ID was
          // generated by the system, as the device did not have a serial number.
          if(String(storage_subroots[ii]).charAt(1)=="&") print_table_row("Unique ID:",storage_subroots[ii]+" (Generated by system)");
          else print_table_row("Unique ID:",storage_subroots[ii]);

          val=GetRegistryKeyValue(cur_controlset+"\\Enum\\USBSTOR\\"+storage_roots[i]+"\\"+storage_subroots[ii],"Class");
          print_table_row("Class:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "");
          val=GetRegistryKeyValue(cur_controlset+"\\Enum\\USBSTOR\\"+storage_roots[i]+"\\"+storage_subroots[ii],"DeviceDesc");
          print_table_row("Device description:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "");
          val=GetRegistryKeyValue(cur_controlset+"\\Enum\\USBSTOR\\"+storage_roots[i]+"\\"+storage_subroots[ii],"FriendlyName");
          print_table_row("Friendly name:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "");
          val=GetRegistryKeyValue(cur_controlset+"\\Enum\\USBSTOR\\"+storage_roots[i]+"\\"+storage_subroots[ii],"ParentIdPrefix");
          if(IsValid(val)) {
            // Windows XP uses the ParentId to link to MountedDevices
            var parent_id=RegistryKeyValueToString(val.value,val.type);
            print_table_row("Parent ID prefix:",parent_id);
            // Find mount point(s)
            print("      <tr><td>Mount point(s):</td><td>");
            var br=0;
            for(var iii=0;iii<mnt_keys.length;iii++) {
              if(String(mnt_values[iii]).indexOf("#"+parent_id+"&")!=-1) {
                if(br==1) print("<br />");
                else br=1;
                print(mnt_keys[iii]);
              }
            }
            if(br==0) print("n/a");
            println("</td></tr>");
          } else {
            // Since Vista, Unique IDs are used
            // Find mount point(s)
            print("      <tr><td>Mount point(s):</td><td>");
            var br=0;
            for(var iii=0;iii<mnt_keys.length;iii++) {
              if(String(mnt_values[iii]).indexOf("#"+storage_subroots[ii]+"#")!=-1) {
                if(br==1) print("<br />");
                else br=1;
                print(mnt_keys[iii]);
              }
            }
            if(br==0) print("n/a");
            println("</td></tr>");
          }
          println("    </table>");
          println("    <br />");
        }
      }
    } else {
      println("  <font color='red'>This registry hive does not contain a list of attached USB storage devices!</font>");
    }
    println("  </p>");
  } else {
    println("  <p><font color='red'>");
    println("    Unable to determine current control set!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }
}
