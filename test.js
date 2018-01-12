const aeronjs = require('bindings')('aeronjs');
const dgram = require('dgram') 

const streamId = 37720  // TODO
aeronjs('/opt/yi/data/aeron', 'aeron:udp?endpoint=10.0.10.81:5001', streamId, function(msg){
  console.log('callback: ' + msg); // 'hello world'
});

const udpServer = dgram.createSocket('udp4')
udpServer.bind(12222)
