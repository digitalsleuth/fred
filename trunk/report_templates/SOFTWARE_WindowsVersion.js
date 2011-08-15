println("<html>");
println("  <head><title>Windows version info</title></head>");
println("  <body>");
println("  <h2>Windows version info</h2>");
println("  <p>");

var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","ProductName");
var value_len=val.length;
print("ProductVersion: ");
println(RegistryKeyValueToString(val.value,val.type));


//for(var i=0;i<(val.length-1);i+=2) {
//  print(String.fromCharCode(val.value[i]));
//}

println("  </p>");
println("</html>");

