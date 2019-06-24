var NAVTREE =
[
  [ "FieldTalk Modbus Master C++ Library", "index.html", [
    [ "Introduction", "index.html", null ],
    [ "Chapters", "pages.html", [
      [ "What You should know about Modbus", "modbus.html", null ],
      [ "Installation and Source Code Compilation", "install.html", null ],
      [ "Linking your Applications against the Library", "linking.html", null ],
      [ "How to integrate the Protocol in your Application", "integrate.html", null ],
      [ "Examples", "examples.html", null ],
      [ "Design Background", "design.html", null ],
      [ "License", "license.html", null ],
      [ "Support", "support.html", null ],
      [ "Notices", "notices.html", null ]
    ] ],
    [ "Modules", "modules.html", [
      [ "Data and Control Functions for all Modbus Protocol Flavours", "group__mbusmaster.html", null ],
      [ "TCP/IP Protocols", "group__mbusmastertcp.html", [
        [ "Classes", "group__mbusmastertcp.html", [
          [ "MbusTcpMasterProtocol", "classMbusTcpMasterProtocol.html", null ],
          [ "MbusRtuOverTcpMasterProtocol", "classMbusRtuOverTcpMasterProtocol.html", null ]
        ] ]
      ] ],
      [ "Serial Protocols", "group__mbusmasterserial.html", [
        [ "Classes", "group__mbusmasterserial.html", [
          [ "MbusRtuMasterProtocol", "classMbusRtuMasterProtocol.html", null ],
          [ "MbusAsciiMasterProtocol", "classMbusAsciiMasterProtocol.html", null ]
        ] ]
      ] ],
      [ "Error Management", "group__buserror.html", [
        [ "Defines", "group__buserror.html", [
          [ "FTALK_SUCCESS", "group__buserror.html#ga30cbe7caf80689adf2b4359abbddd046", null ],
          [ "FTALK_ILLEGAL_ARGUMENT_ERROR", "group__buserror.html#ga7fff571866a603e64149d161ef4834b7", null ],
          [ "FTALK_ILLEGAL_STATE_ERROR", "group__buserror.html#gab81ed2a5ebe59e36b1b2e7060eec9c94", null ],
          [ "FTALK_EVALUATION_EXPIRED", "group__buserror.html#ga21dbd89ee9ec549e2f8689ed268e0c37", null ],
          [ "FTALK_NO_DATA_TABLE_ERROR", "group__buserror.html#ga17e30e58a314f3d76f4c35c5641b4a03", null ],
          [ "FTALK_ILLEGAL_SLAVE_ADDRESS", "group__buserror.html#gad5bd16c6b3ae5d2f46a5b2be3b4e0be5", null ],
          [ "FTALK_IO_ERROR_CLASS", "group__buserror.html#gad22c279dab176ee34f80b74c85c788a3", null ],
          [ "FTALK_IO_ERROR", "group__buserror.html#ga4411354a1b4c927b112574cdd1d0329f", null ],
          [ "FTALK_OPEN_ERR", "group__buserror.html#ga75e48a0b5bc8d8f3bcbe2a5bf37204f8", null ],
          [ "FTALK_PORT_ALREADY_OPEN", "group__buserror.html#ga686033ad6c0a15512d2fa02b6a61783b", null ],
          [ "FTALK_TCPIP_CONNECT_ERR", "group__buserror.html#ga1eae81706e0d82638cec59384806ea9b", null ],
          [ "FTALK_CONNECTION_WAS_CLOSED", "group__buserror.html#ga9b1d1a0cdc1d90337ff6250b64c4c688", null ],
          [ "FTALK_SOCKET_LIB_ERROR", "group__buserror.html#gac173359ca68af812a49e5c6e4b5258d4", null ],
          [ "FTALK_PORT_ALREADY_BOUND", "group__buserror.html#ga99f8429f030f71d09eae96918916d0f2", null ],
          [ "FTALK_LISTEN_FAILED", "group__buserror.html#gaf70581cb491c094e15d213dc5319bc9c", null ],
          [ "FTALK_FILEDES_EXCEEDED", "group__buserror.html#ga9416f8b068e8457b5330cb8a5be2d68d", null ],
          [ "FTALK_PORT_NO_ACCESS", "group__buserror.html#ga53285b1059569670416fff640dd5dda6", null ],
          [ "FTALK_PORT_NOT_AVAIL", "group__buserror.html#gaa11ccf27d61cc78e9d8ed7f656ddc7ce", null ],
          [ "FTALK_LINE_BUSY_ERROR", "group__buserror.html#ga3469a60dcf23c8968cf0180aacb9109c", null ],
          [ "FTALK_BUS_PROTOCOL_ERROR_CLASS", "group__buserror.html#ga7a803e248c41302840874c176384b84c", null ],
          [ "FTALK_CHECKSUM_ERROR", "group__buserror.html#gaaac6a1b18ca581f79a94897ed55d4937", null ],
          [ "FTALK_INVALID_FRAME_ERROR", "group__buserror.html#ga5a324513e2bd1f0228586db3b52173e7", null ],
          [ "FTALK_INVALID_REPLY_ERROR", "group__buserror.html#ga3d06e7ad0e3b4e776344a1482a518550", null ],
          [ "FTALK_REPLY_TIMEOUT_ERROR", "group__buserror.html#ga2de1670af78a3e84555948318d888fba", null ],
          [ "FTALK_SEND_TIMEOUT_ERROR", "group__buserror.html#gae6d54df056ad61bd6709e547b50107ac", null ],
          [ "FTALK_INVALID_MBAP_ID", "group__buserror.html#gae27245183b7c84f7f9f7019b34663507", null ],
          [ "FTALK_MBUS_EXCEPTION_RESPONSE", "group__buserror.html#gae9594b8c478cd52379c09a64cb957d3e", null ],
          [ "FTALK_MBUS_ILLEGAL_FUNCTION_RESPONSE", "group__buserror.html#gaed34db07d555239a824f4e6fe1083128", null ],
          [ "FTALK_MBUS_ILLEGAL_ADDRESS_RESPONSE", "group__buserror.html#ga7569e87896275477e2fcf037da835fdf", null ],
          [ "FTALK_MBUS_ILLEGAL_VALUE_RESPONSE", "group__buserror.html#ga245ddab7c63b5a20f20faff7fb9aa82c", null ],
          [ "FTALK_MBUS_SLAVE_FAILURE_RESPONSE", "group__buserror.html#gabb2df138c98aab4b74ab81b379d8c593", null ],
          [ "FTALK_MBUS_GW_PATH_UNAVAIL_RESPONSE", "group__buserror.html#ga8ce2e58606bb72e0d04cc0326459e7b5", null ],
          [ "FTALK_MBUS_GW_TARGET_FAIL_RESPONSE", "group__buserror.html#gabd9800cdb3ae6941468057906e5db04b", null ]
        ] ],
        [ "Functions", "group__buserror.html", [
          [ "getBusProtocolErrorText", "group__buserror.html#ga86619062eb7bb909d9600901b457143f", null ]
        ] ]
      ] ],
      [ "Device and Vendor Specific Modbus Functions", "group__devicespecific.html", [
        [ "Functions", "group__devicespecific.html", [
          [ "adamSendReceiveAsciiCmd", "group__devicespecific.html#ga2dc723b750f16298e186e23790e6c783", null ],
          [ "customFunction", "group__devicespecific.html#gaea9132747bc347fe286d9f7f66d96ee1", null ]
        ] ]
      ] ]
    ] ],
    [ "Class List", "annotated.html", [
      [ "MbusAsciiMasterProtocol", "classMbusAsciiMasterProtocol.html", null ],
      [ "MbusMasterFunctions", "classMbusMasterFunctions.html", null ],
      [ "MbusRtuMasterProtocol", "classMbusRtuMasterProtocol.html", null ],
      [ "MbusRtuOverTcpMasterProtocol", "classMbusRtuOverTcpMasterProtocol.html", null ],
      [ "MbusSerialMasterProtocol", "classMbusSerialMasterProtocol.html", null ],
      [ "MbusTcpMasterProtocol", "classMbusTcpMasterProtocol.html", null ]
    ] ],
    [ "Class Index", "classes.html", null ],
    [ "Class Hierarchy", "hierarchy.html", [
      [ "MbusMasterFunctions", "classMbusMasterFunctions.html", [
        [ "MbusSerialMasterProtocol", "classMbusSerialMasterProtocol.html", [
          [ "MbusAsciiMasterProtocol", "classMbusAsciiMasterProtocol.html", null ],
          [ "MbusRtuMasterProtocol", "classMbusRtuMasterProtocol.html", null ]
        ] ],
        [ "MbusTcpMasterProtocol", "classMbusTcpMasterProtocol.html", [
          [ "MbusRtuOverTcpMasterProtocol", "classMbusRtuOverTcpMasterProtocol.html", null ]
        ] ]
      ] ]
    ] ],
    [ "Class Members", "functions.html", null ]
  ] ]
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}

