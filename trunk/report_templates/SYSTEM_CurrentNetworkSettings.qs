function IsValid(val) {
  if(typeof val !== 'undefined') return true;
  else return false;
}

function print_table_row(cell01,cell02) {
  println("      <tr><td>",cell01,"</td><td>",cell02,"</td></tr>");
}

function ZeroPad(number,padlen) {
  var ret=number.toString(10);
  if(!padlen || ret.length>=padlen) return ret;
  return Math.pow(10,padlen-ret.length).toString().slice(1)+ret;
}

// Global vars
var val;

println("<html>");
println("  <head><title>Current Network Settings (Tcp/Ip)</title></head>");
println("  <body style=\"font-size:12\">");
println("  <h2>Current network settings (Tcp/Ip)</h2>");

// Get current controlset
var cur_controlset=GetRegistryKeyValue("\\Select","Current");
if(IsValid(cur_controlset)) {
  cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
  // Current holds a DWORD value, thus we get a string like 0x00000000, but
  // control sets are referenced by its decimal representation.
  cur_controlset="ControlSet"+ZeroPad(parseInt(String(cur_controlset).substr(2,8),16),3)

  println("  <p style=\"font-size:12; white-space:nowrap\">");
  println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");
  print_table_row("Active control set:",cur_controlset);

  // Computer name
  val=GetRegistryKeyValue(cur_controlset+"\\Control\\ComputerName\\ComputerName","ComputerName");
  print_table_row("Computer name:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "");

  println("    </table>");
  println("    <br />");

  // Iterate over all available network adapters
  var adapters=GetRegistryNodes(cur_controlset+"\\Services\\Tcpip\\Parameters\\Adapters");
  for(var i=0;i<adapters.length;i++) {
    // TODO: Try to get a human readable name
    println("    ",adapters[i]);
    // Get settings node
    var adapter_settings_node=GetRegistryKeyValue(cur_controlset+"\\Services\\Tcpip\\Parameters\\Adapters\\"+adapters[i],"IpConfig");
    adapter_settings_node=RegistryKeyValueToVariant(adapter_settings_node.value,"utf16",0);

    println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

    // Get configuration mode
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"EnableDHCP");
    val=Number(RegistryKeyValueToString(val.value,val.type));
    if(val) {
      // DHCP enabled
      print_table_row("Configuration mode:","DHCP");
      // DHCP server
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpServer");
      print_table_row("Last used DHCP server:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "");
      // IP address
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpIPAddress");
      print_table_row("IP address:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "");
      // Subnet mask
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpSubnetMask");
      print_table_row("Subnet mask:",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "");
      // Nameserver(s)
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpNameServer");
      print_table_row("Nameserver(s):",(IsValid(val)) ? RegistryKeyValueToString(val.value,val.type) : "");
      // Default gw
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpDefaultGateway");
      print_table_row("Default gateway:",(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
      // Lease obtained
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"LeaseObtainedTime");
      print_table_row("Lease obtained:",(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"unixtime",0) : "");
      // Lease valid until
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"LeaseTerminatesTime");
      print_table_row("Lease terminates:",(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"unixtime",0) : "");
    } else {
      print_table_row("Configuration mode:","Manual");
      // IP address
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"IPAddress");
      print_table_row("IP address:",(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
      // Subnet mask
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"SubnetMask");
      print_table_row("Subnet mask:",(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
      // Nameserver
      // TODO: Has to be validated
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"NameServer");
      print_table_row("Nameserver:",(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
      // Default gw
      val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DefaultGateway");
      print_table_row("Default gateway:",(IsValid(val)) ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
    }

    println("    </table>");
    println("    <br />");
  }
  println("  </p>");
} else {
  println("  <p><font color='red'>");
  println("    Unable to determine current control set!<br />");
  println("    Are you sure you are running this report against the correct registry hive?");
  println("  </font></p>");
}

println("</html>");
