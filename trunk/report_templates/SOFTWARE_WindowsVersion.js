println("<html>");
println("  <head><title>Windows version info</title></head>");
println("  <body>");
println("  <h2>Windows version info</h2>");
println("  <p>");

var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","ProductName");

print("ProductVersion: ");
var str_value=RegistryKeyValueToString(val.value,val.type);
println(str_value);

println("  </p>");
println("</html>");

//for(var i=0;i<(val.length-1);i+=2) {
//  print(String.fromCharCode(val.value[i]));
//}

