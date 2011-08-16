println("<html>");
println("  <head><title>Windows version info</title></head>");
println("  <body>");
println("  <h2>Windows version info</h2>");
println("  <p>");

// Windows version
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","ProductName");
println("Windows version: ",RegistryKeyValueToString(val.value,1),"<br />");

// Install date
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","InstallDate");
println("Install date: ",RegistryKeyValueToVariant(val.value,"unixtime"),"<br />");

// Owner and Organization info
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","RegisteredOwner");
println("Registered owner: ",RegistryKeyValueToString(val.value,1),"<br />");
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","RegisteredOrganization");
println("Registered organization: ",RegistryKeyValueToString(val.value,1),"<br />");

println("  </p>");
println("</html>");
