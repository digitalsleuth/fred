println("<html>");
println("  <head><title>Recent Documents</title></head>");
println("  <body style=\"font-size:12\">");
println("  <h2>Recent documents</h2>");
println("  <p style=\"font-size:12\">");
println("    <table style=\"margin-left:20px; font-size:12\">");

// Get list of recent docs
var recent_docs=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs","MRUListEx");

// Iterate over all recent docs
var i=0;
var runlist=RegistryKeyValueToVariant(recent_docs.value,"uint32",i);
while(Number(runlist)!=0xffffffff) {
  var entry=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs",runlist.toString(10));
  println("      <tr><td>",RegistryKeyValueToVariant(entry.value,"utf16",0),"</td></tr>");
  i+=4;
  runlist=RegistryKeyValueToVariant(recent_docs.value,"uint32",i);
}

println("    </table>");
println("  </p>");
println("</html>");
