// See http://windowsir.blogspot.com/2006/08/getting-user-info-from-image.html

println("<html>");
println("  <head><title>User Accounts</title></head>");
println("  <body>");
println("  <h2>User accounts</h2>");

// Iterate over all user names
var user_names=GetRegistryNodes("\\SAM\\Domains\\Account\\Users\\Names");
for(var i=0;i<user_names.length;i++) {
  println("  <p>");

  // Print user name
  println("    ",user_names[i],"<br />");

  println("    <table style=\"margin-left:20px\">");

  // Get user rid stored in "default" key
  var user_rid=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\Names\\",user_names[i]),"");
  user_rid=RegistryKeyTypeToString(user_rid.type);
  println("<tr><td>RID:</td><td>",Number(user_rid).toString(10)," (",user_rid,")","</td></td>");

  // RegistryKeyTypeToString returns the rid prepended with "0x". We have to remove that for further processing
  user_rid=String(user_rid).substr(2);

  // Get user's V key and print various infos
  var v_key=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"V");

  // Get user's F key and print various infos
  var f_key=GetRegistryKeyValue(String().concat("\\SAM\\Domains\\Account\\Users\\",user_rid),"F");
  println("<tr><td>Last login time:</td><td>",RegistryKeyValueToVariant(f_key.value,"filetime",8),"</td></td>");
  println("<tr><td>Last pw change:</td><td>",RegistryKeyValueToVariant(f_key.value,"filetime",24),"</td></td>");
  println("<tr><td>Last failed login:</td><td>",RegistryKeyValueToVariant(f_key.value,"filetime",40),"</td></td>");
  println("<tr><td>Account expires:</td><td>",RegistryKeyValueToVariant(f_key.value,"filetime",32),"</td></td>");
  
  println("<tr><td>Total logins:</td><td>",RegistryKeyValueToVariant(f_key.value,"uint16",66),"</td></td>");
  println("<tr><td>Failed logins:</td><td>",RegistryKeyValueToVariant(f_key.value,"uint16",64),"</td></td>");
  
  var acc_flags=Number(RegistryKeyValueToVariant(f_key.value,"uint16",56));
  print("<tr><td>Account flags:</td><td>");
  if(acc_flags&0x0001) print("Disabled ");
  if(acc_flags&0x0002) print("HomeDirReq ");
  if(acc_flags&0x0004) print("PwNotReq ");
  if(acc_flags&0x0008) print("TempDupAcc ");
  // Don't think this would be useful to show
  //if(acc_flags&0x0010) print("NormUserAcc ");
  if(acc_flags&0x0020) print("MnsAcc ");
  if(acc_flags&0x0040) print("DomTrustAcc ");
  if(acc_flags&0x0080) print("WksTrustAcc ");
  if(acc_flags&0x0100) print("SrvTrustAcc ");
  if(acc_flags&0x0200) print("NoPwExpiry ");
  if(acc_flags&0x0400) print("AccAutoLock ");
  println("</td></td>");
  
  
  
  
  println("  </table>");


  println("  </p>");
}

println("</html>");
