function fred_report_info() {
  var info={report_cat    : "SOFTWARE",
            report_name   : "Profile list",
            report_author : "Gillen Daniel",
            report_desc   : "Dump profile list",
            fred_api      : 2,
            hive          : "SOFTWARE"
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

function fred_report_html() {
  var val;

  println("<html>");
  println("  <head><title>Profile List</title></head>");
  println("  <body style=\"font-size:12\">");
  println("  <h2>Profile List</h2>");

  var profile_list=GetRegistryNodes("\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList");
  if(IsValid(profile_list) && profile_list.length>0) {
    for(var i=0;i<profile_list.length;i++) {
      println("  <p style=\"font-size:12; white-space:nowrap\">");
      println("    <u>"+profile_list[i]+"</u><br />");
      println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

      // Get profile image path
      val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\"+profile_list[i],"ProfileImagePath");
      print_table_row("Profile image path:",IsValid(val) ? RegistryKeyValueToString(val.value,val.type) : "n/a");

      // Get last load time (Saved as 2 dwords. Another "good" idea of M$ ;-))
      var loadtime_low=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\"+profile_list[i],"ProfileLoadTimeLow");
      var loadtime_high=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\"+profile_list[i],"ProfileLoadTimeHigh");
      print_table_row("Profile load time:",(IsValid(loadtime_low) && IsValid(loadtime_high)) ? RegistryKeyValueToVariant(loadtime_low.value.append(loadtime_high.value),"filetime",0) : "n/a");

      // TODO: There is more to decode under \\Microsoft\\Windows NT\\CurrentVersion\\ProfileList

      println("    </table>");
      println("  </p>");
    }
    println("    </table>");
  } else {
    println("    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None");
  }

  println("</html>");
}
