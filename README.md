# dojoQt
QT implementation of dojo project.

Dojo is a system which should implement brain-like behaviour.

It requires Redis http://redis.io/ as a backend storage.

Example project in /examples/dojoApp project.

Simple usage:
  
      dojoNetwork network;
      
      //setting up network
      dojoID node1 = network.createNode(QVector3D(1,1,0),QVector3D(1,1,2), 0.98);
      dojoID node2 = network.createNode(QVector3D(1,1,3),QVector3D(1,1,4), 1.1);
      network.bindNodes(node1,node2);
      
      //let's go
      network.start();
      
      
Admin tools in /html/index.html - work through Websockets




