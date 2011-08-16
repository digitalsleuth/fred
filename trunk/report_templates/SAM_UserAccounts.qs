// See http://windowsir.blogspot.com/2006/08/getting-user-info-from-image.html

println("<html>");
println("  <head><title>User Accounts</title></head>");
println("  <body>");
println("  <h2>User accounts</h2>");
println("  <p>");

// Iterate over all user names
var user_names=GetRegistryNodes("\\SAM\\Domains\\Account\\Users\\Names");
for(var i=0;i<user_names.length;i++) {
  // Print user name
  println(user_names[i],"<br />");

  // Get user rid stored in "default" key
  var user_rid=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\Names\\",user_names[i]),"");
  user_rid=RegistryKeyTypeToString(user_rid.type);
  println("&nbsp;&nbsp;RID: ",Number(user_rid).toString(10)," (",user_rid,")","<br />");

  // RegistryKeyTypeToString returns the rid prepended with "0x". We have to remove that for further processing
  user_rid=String(user_rid).substr(2);
  
  // Get user's V key and print various infos
  var v_key=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"V");
  

  // Get user's F key and print various infos
  var f_key=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"F");
  println("&nbsp;&nbsp;Last lockout time: ",RegistryKeyValueToVariant(f_key.value,"filetime",8),"<br />");
  println("&nbsp;&nbsp;Creation time: ",RegistryKeyValueToVariant(f_key.value,"filetime",24),"<br />");
  println("&nbsp;&nbsp;Last login time: ",RegistryKeyValueToVariant(f_key.value,"filetime",40),"<br />");

  println("<br />");
}

println("  </p>");
println("</html>");
