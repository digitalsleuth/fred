println("<html>");
println("  <head><title>Windows Live Accounts</title></head>");
println("  <body style=\"font-size:12\">");
println("  <h2>Windows live accounts</h2>");
println("  <p style=\"font-size:12\">");
println("    <table style=\"margin-left:20px; font-size:12\">");

// Iterate over all contacts
var accounts=GetRegistryKeys("\\Software\\Microsoft\\Windows Live Contacts\\Database");
for(var i=0;i<accounts.length;i++) {
  var val=GetRegistryKeyValue("\\Software\\Microsoft\\Windows Live Contacts\\Database",accounts[i]);
  println("      <tr><td>",accounts[i],"</td><td>",RegistryKeyValueToString(val.value,val.type),"</td></tr>");
}
accounts=GetRegistryKeys("\\Software\\Microsoft\\Windows Live Contacts\\Me");
for(var i=0;i<accounts.length;i++) {
  var val=GetRegistryKeyValue("\\Software\\Microsoft\\Windows Live Contacts\\Me",accounts[i]);
  println("      <tr><td>",accounts[i],"</td><td>",RegistryKeyValueToString(val.value,val.type),"</td></tr>");
}

println("    </table>");
println("  </p>");
println("</html>");
