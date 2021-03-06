function print_table_row(cell01,cell02) {
  println("      <tr><td>",cell01,"</td><td>",cell02,"</td></tr>");
}

// Global vars
var val;

// Get current controlset
var cur_controlset=GetRegistryKeyValue("\\Select","Current");
cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
// Current holds a DWORD value, thus we get a string like 0x00000000, but
// control sets are referenced only with the last 3 digits.
cur_controlset="ControlSet"+String(cur_controlset).substr(7,3);

println("<html>");
println("  <head><title>Current Network Settings (Tcp/Ip)</title></head>");
println("  <body style=\"font-size:12\">");
println("  <h2>Current network settings (Tcp/Ip)</h2>");
println("  <p style=\"font-size:12; white-space:nowrap\">");
println("    <table style=\"margin-left:20px; font-size:12; white-space:nowrap\">");

print_table_row("Active control set:",cur_controlset);

// Computer name
val=GetRegistryKeyValue(cur_controlset+"\\Control\\ComputerName\\ComputerName","ComputerName");
print_table_row("Computer name:",RegistryKeyValueToString(val.value,val.type));

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
    print_table_row("Last used DHCP server:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "");
    // IP address
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpIPAddress");
    print_table_row("IP address:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "");
    // Subnet mask
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpSubnetMask");
    print_table_row("Subnet mask:",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "");
    // Nameserver(s)
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpNameServer");
    print_table_row("Nameserver(s):",(typeof val !== 'undefined') ? RegistryKeyValueToString(val.value,val.type) : "");
    // Default gw
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DhcpDefaultGateway");
    print_table_row("Default gateway:",(typeof val !== 'undefined') ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
    // Lease obtained
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"LeaseObtainedTime");
    print_table_row("Lease obtained:",(typeof val !== 'undefined') ? RegistryKeyValueToVariant(val.value,"unixtime",0) : "");
    // Lease valid until
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"LeaseTerminatesTime");
    print_table_row("Lease terminates:",(typeof val !== 'undefined') ? RegistryKeyValueToVariant(val.value,"unixtime",0) : "");
  } else {
    print_table_row("Configuration mode:","Manual");
    // IP address
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"IPAddress");
    print_table_row("IP address:",(typeof val !== 'undefined') ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
    // Subnet mask
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"SubnetMask");
    print_table_row("Subnet mask:",(typeof val !== 'undefined') ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
    // Nameserver
    // TODO: Has to be validated
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"NameServer");
    print_table_row("Nameserver:",(typeof val !== 'undefined') ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
    // Default gw
    val=GetRegistryKeyValue(cur_controlset+"\\Services\\"+adapter_settings_node,"DefaultGateway");
    print_table_row("Default gateway:",(typeof val !== 'undefined') ? RegistryKeyValueToVariant(val.value,"utf16",0) : "");
  }

  println("    </table>");
  println("    <br />");
}

println("  </p>");
println("</html>");
