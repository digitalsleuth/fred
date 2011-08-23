function print_table_row(cell01,cell02) {
  println("      <tr><td>",cell01,"</td><td>",cell02,"</td></tr>");
}

function DecodeProductKey(arr) {
  //ProductKey is base24 encoded
  var keychars=new Array("B","C","D","F","G","H","J","K","M","P","Q","R","T","V","W","X","Y","2","3","4","6","7","8","9");
  var key=new Array(30);
  var ret="";
  var ncur;

  if(arr.length<66) return ret;

  arr=arr.mid(52,15);
  for(var ilbyte=24;ilbyte>=0;ilbyte--) {
    ncur=0;
    for(var ilkeybyte=14;ilkeybyte>=0;ilkeybyte--) {
      ncur=ncur*256^arr[ilkeybyte];
      arr[ilkeybyte]=ncur/24;
      ncur%=24;
    }
    ret=keychars[ncur]+ret;
    if(ilbyte%5==0 && ilbyte!=0) ret="-"+ret;
  }
  return ret;
}

println("<html>");
println("  <head><title>Windows version info</title></head>");
println("  <body style=\"font-size:12\">");
println("  <h2>Windows version info</h2>");
println("  <p style=\"font-size:12; white-space:nowrap\">");
println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

// Windows version sp and build info
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","ProductName");
print("        <tr><td>Windows version:</td><td>",RegistryKeyValueToString(val.value,val.type));
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","CSDVersion");
if(typeof val !== 'undefined') {
  print(" ",RegistryKeyValueToString(val.value,val.type));
}
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","CurrentBuildNumber");
if(typeof val !== 'undefined') {
  print(" build ",RegistryKeyValueToString(val.value,val.type));
}
println("</td></tr>");
// Build string
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","BuildLab");
print_table_row("Build string:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");
// Extended build string
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","BuildLabEx");
print_table_row("Extended build string:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");

// Install date
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","InstallDate");
print_table_row("Install date:",(typeof val !== 'undefined') ? RegistryKeyValueToVariant(val.value,"unixtime") : "n/a");

// Owner and Organization info
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","RegisteredOwner");
print_table_row("Registered owner:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","RegisteredOrganization");
print_table_row("Registered organization:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");

// Windows ID / Key
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","ProductId");
print_table_row("Product ID:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","DigitalProductId");
if(typeof val !== 'undefined') {
  var key=DecodeProductKey(val.value);
  if(key!="BBBBB-BBBBB-BBBBB-BBBBB-BBBBB") print_table_row("Product Key:",key);
  else print_table_row("Product Key:","n/a (Probably a volume license key was used)");
} else print_table_row("Product Key:","n/a");

// Install directory / Source directory
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","PathName");
print_table_row("Install path:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");
var val=GetRegistryKeyValue("\\Microsoft\\Windows NT\\CurrentVersion","SourcePath");
print_table_row("Source path:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "n/a");

println("    </table>");
println("  </p>");
println("</html>");
