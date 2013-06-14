function fred_report_info() {
  var info={report_cat    : "SYSTEM",
            report_name   : "Backup / Restore settings",
            report_author : "Gillen Daniel",
            report_desc   : "Dump files / directories not to snapshot / backup and registry keys not to restore",
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

function ListValues(root_key) {
  var values=GetRegistryKeys(root_key);
  if(IsValid(values)) {
    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");
    for(var i=0;i<values.length;i++) {
      var val=GetRegistryKeyValue(root_key,values[i]);
      if(IsValid(val)) {
        println("      <tr>");
        println("        <td>",values[i],"</td>");
        println("        <td>");
        var strings=RegistryKeyValueToStringList(val.value);
        for(var ii=0;ii<strings.length;ii++) {
          println("          ",strings[ii],"<br />");
        }
        println("        </td>");
        println("      </tr>");
      }
    }
    println("    </table>");
    println("  </p>");
  } else {
    println("  None");
  }
}

function fred_report_html() {
  var val;

  println("  <h2>Backup / Restore settings</h2>");

  // Get current controlset
  var cur_controlset=GetRegistryKeyValue("\\Select","Current");
  if(IsValid(cur_controlset)) {
    cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
    // Current holds a DWORD value, thus we get a string like 0x00000000, but
    // control sets are referenced only with the last 3 digits.
    cur_controlset="ControlSet"+String(cur_controlset).substr(7,3);
    
    println("  <u>Directories / files not to back up in Volume Shadow Copies</u>");
    ListValues(cur_controlset+"\\Control\\BackupRestore\\FilesNotToSnapshot");
    println("  <u>Directories / files not to back up or restore by backup apps</u>");
    ListValues(cur_controlset+"\\Control\\BackupRestore\\FilesNotToBackup");
    println("  <u>Registry nodes or values not to restore by backup apps</u>");
    ListValues(cur_controlset+"\\Control\\BackupRestore\\KeysNotToRestore");
  } else {
    println("  <p><font color='red'>");
    println("    Unable to determine current control set!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }
}
