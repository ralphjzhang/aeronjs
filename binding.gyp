{
  "targets": [
    {
      "target_name": "aeronjs",
      "cflags": [ "-Wall", "-std=c++11" ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "aeronjs.cpp" ],
      "include_dirs": [
        "<!@(node -p \"require('nan').include\")"
      ],
      "libraries": [ "/home/devops/labs/aeronjs/libaeron_client_cgo.a" ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}