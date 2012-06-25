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

// Global vars
var val;

// Get current controlset
var cur_controlset=GetRegistryKeyValue("\\Select","Current");
cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
// Current holds a DWORD value, thus we get a string like 0x00000000, but
// control sets are referenced only with the last 3 digits.
cur_controlset="ControlSet"+String(cur_controlset).substr(7,3);

println("<html>");
println("  <head><title>System Time Info</title></head>");
println("  <body style=\"font-size:12\">");
println("  <h2>System time info (",cur_controlset,")</h2>");
println("  <p style=\"font-size:12; white-space:nowrap\">");
println("    <u>Time zone info</u>");
println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

// Active time bias
val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","ActiveTimeBias");
print_table_row("Active time bias:",(typeof val !== 'undefined') ? ToUTC(RegistryKeyValueToString(val.value,val.type)) : "n/a");

// Std. tz name and bias
val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","StandardName");
print_table_row("Std. time zone name:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");
val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","StandardBias");
print_table_row("Std. time bias:",(typeof val !== 'undefined') ? ToUTC(RegistryKeyValueToString(val.value,val.type)) : "n/a");

// Daylight tz name and bias
val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","DaylightName");
print_table_row("Daylight time zone name:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");
val=GetRegistryKeyValue(cur_controlset+"\\Control\\TimeZoneInformation","DaylightBias");
print_table_row("Daylight time bias:",(typeof val !== 'undefined') ? ToUTC(RegistryKeyValueToString(val.value,val.type)) : "n/a");

println("    </table>");
println("    <br />");
println("    <u>W32Time service info</u>");
println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

// Get W32Time service settings
val=GetRegistryKeyValue(cur_controlset+"\\Services\\W32Time","Start");
if(typeof val !== 'undefined') {
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
    print_table_row("NTP server(s):",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");
  }
} else print_table_row("Startup method:","n/a");

println("    </table>");
println("  </p>");
println("</html>");
