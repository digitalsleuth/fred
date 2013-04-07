function fred_report_info() {
  var info={report_cat    : "SYSTEM",
            report_name   : "System time info",
            report_author : "Gillen Daniel",
            report_desc   : "Dump system time info",
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

function ToUTC(num) {
  var retnum=new Number(num);
  if(retnum&0x80000000) {
    retnum=((0xFFFFFFFF-retnum)+1)/60;
    return "UTC+"+Number(retnum).toString(10);
  } else {
    retnum=retnum/60;
    if(retnum!=0) return "UTC-"+Number(retnum).toString(10);
    else return "UTC+"+Number(retnum).toString(10);
  }
}

function ZeroPad(number,padlen) {
  var ret=number.toString(10);
  if(!padlen || ret.length>=padlen) return ret;
  return Math.pow(10,padlen-ret.length).toString().slice(1)+ret;
}

function fred_report_html() {
  var val;

  println("<html>");
  println("  <head><title>System Time Info</title></head>");
  println("  <body style=\"font-size:12\">");
  println("  <h2>System time info (",cur_controlset,")</h2>");

  // Get current controlset
  var cur_controlset=GetRegistryKeyValue("\\Select","Current");
  if(IsValid(cur_controlset)) {
    cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
    // Current holds a DWORD value, thus we get a string like 0x00000000, but
    // control sets are referenced by its decimal representation.
    cur_controlset="ControlSet"+ZeroPad(parseInt(String(cur_controlset).substr(2,8),16),3)

    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <u>Time zone info</u>");
    println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

    // Active time bias
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","ActiveTimeBias");
    print_table_row("Active time bias:",(IsValid(val)) ? ToUTC(RegistryKeyValueToString(val.value,val.type)) : "n/a");

    // Std. tz name and bias
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","StandardName");
    print_table_row("Std. time zone name:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","StandardBias");
    print_table_row("Std. time bias:",(IsValid(val)) ? ToUTC(RegistryKeyValueToString(val.value,val.type)) : "n/a");

    // Daylight tz name and bias
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","DaylightName");
    print_table_row("Daylight time zone name:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");
    val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","DaylightBias");
    print_table_row("Daylight time bias:",(IsValid(val)) ? ToUTC(RegistryKeyValueToString(val.value,val.type)) : "n/a");

    println("    </table>");
    println("    <br />");
    println("    <u>W32Time service info</u>");
    println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

    // Get W32Time service settings
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\W32Time","Start");
    if(IsValid(val)) {
      print("      <tr><td>Startup method:</td><td>");
      val=RegistryKeyValueToString(val.value,val.type);
      switch(Number(val)) {
        case 0:
          print("Boot");
          break;
        case 1:
          print("System");
          break;
        case 2:
          print("Automatic");
          break;
        case 3:
          print("Manual");
          break;
        case 4:
          print("Disabled");
          break;
        default:
          print("Unknown");
      }
      println("</td></tr>");
      // If service is enabled, get ntp server
      if(Number(val)<4) {
        val=GetRegistryKeyValue(cur_controlset+"\\Services\\W32Time\\Parameters","NtpServer");
        print_table_row("NTP server(s):",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "n/a");
      }
    } else print_table_row("Startup method:","n/a");

    println("    </table>");
    println("  </p>");
  } else {
    println("  <p><font color='red'>");
    println("    Unable to determine current control set!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }

  println("</html>");
}
