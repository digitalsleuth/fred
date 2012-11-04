function IsValid(val) {
  if(typeof val !== 'undefined') return true;
  else return false;
}

println("<html>");
println("  <head><title>Typed Urls</title></head>");
println("  <body style=\"font-size:12\">");
println("  <h2>Typed urls</h2>");

// Iterate over all typed urls
var typed_urls=GetRegistryKeys("\\Software\\Microsoft\\Internet Explorer\\TypedURLs");
if(IsValid(typed_urls)) {
  println("  <p style=\"font-size:12\">");
  println("    <table style=\"margin-left:20px; font-size:12\">");
  println("      <tr><td><b>Url</b></td><td>&nbsp;&nbsp;<b>Last modified</b></td></tr>");

  for(var i=0;i<typed_urls.length;i++) {
    var val=GetRegistryKeyValue("\\Software\\Microsoft\\Internet Explorer\\TypedURLs",typed_urls[i]);
    var mod_time=GetRegistryKeyModTime("\\Software\\Microsoft\\Internet Explorer\\TypedURLs",typed_urls[i]);
    println("      <tr><td>",RegistryKeyValueToString(val.value,val.type),"</td><td>&nbsp;&nbsp;",mod_time,"</td></tr>");
  }

  println("    </table>");
  println("  </p>");
} else {
  println("  <p><font color='red'>");
  println("    This registry hive does not contain a list of typed urls!");
  println("  </font></p>");
}

println("</html>");
