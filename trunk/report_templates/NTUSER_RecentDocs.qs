println("<html>");
println("  <head><title>Recent Documents</title></head>");
println("  <body>");
println("  <h2>Recent documents</h2>");
println("  <p>");

// Iterate over all recent docs
var recent_docs=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs","MRUListEx");
var i=0;
var runlist=RegistryKeyValueToVariant(recent_docs.value,"uint32",i);

/*
println(runlist.toString(10),"<br />");
var entry=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs",runlist.toString(10));
println("&nbsp;&nbsp;",RegistryKeyValueToVariant(entry.value,"utf16",0),"<br />");
*/

while(Number(runlist)!=0xffffffff) {
  var entry=GetRegistryKeyValue("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs",runlist.toString(10));
  println("&nbsp;&nbsp;",RegistryKeyValueToVariant(entry.value,"utf16",0),"<br />");
  i+=4;
  runlist=RegistryKeyValueToVariant(recent_docs.value,"uint32",i);
}

println("  </p>");
println("</html>");
