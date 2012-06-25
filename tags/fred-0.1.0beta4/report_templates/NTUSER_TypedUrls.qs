println("<html>");
println("  <head><title>Typed Urls</title></head>");
println("  <body style=\"font-size:12\">");
println("  <h2>Typed urls</h2>");
println("  <p style=\"font-size:12\">");
println("    <table style=\"margin-left:20px; font-size:12\">");

// Iterate over all typed urls
var typed_urls=GetRegistryKeys("\\Software\\Microsoft\\Internet Explorer\\TypedURLs");
for(var i=0;i<typed_urls.length;i++) {
  var val=GetRegistryKeyValue("\\Software\\Microsoft\\Internet Explorer\\TypedURLs",typed_urls[i]);
  println("      <tr><td>",RegistryKeyValueToString(val.value,val.type),"</td></tr>");
}

println("    </table>");
println("  </p>");
println("</html>");
