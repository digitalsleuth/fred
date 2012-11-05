function IsValid(val) {
  if(typeof val !== 'undefined') return true;
  else return false;
}

function print_table_row(cell01,cell02) {
  println("      <tr><td>",cell01,"</td><td>",cell02,"</td></tr>");
}

// Global vars
var val;

println("<html>");
println("  <head><title>Last known shutdown time</title></head>");
println("  <body style=\"font-size:12\">");
println("  <h2>Last known shutdown time</h2>");

// Get current controlset
var cur_controlset=GetRegistryKeyValue("\\Select","Current");
if(IsValid(cur_controlset)) {
  cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
  // Current holds a DWORD value, thus we get a string like 0x00000000, but
  // control sets are referenced only with the last 3 digits.
  cur_controlset="ControlSet"+String(cur_controlset).substr(7,3);

  println("  <p style=\"font-size:12; white-space:nowrap\">");
  println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

  print_table_row("Active control set:",cur_controlset);

  // Shutdown time
  val=GetRegistryKeyValue(cur_controlset+"\\Control\\Windows","ShutdownTime");
  print_table_row("Shutdown time:",(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"filetime") : "Unknown");

  println("    </table>");
  println("    <br />");
  println("  </p>");
} else {
  println("  <p><font color='red'>");
  println("    Unable to determine current control set!<br />");
  println("    Are you sure you are running this report against the correct registry hive?");
  println("  </font></p>");
}

println("</html>");
