# bashp

bashp is a hack of bash that outputs shell commands as parsed JSON trees instead
of actually executing.

### Building

    $ git clone --recursive https://github.com/adsr/bashp.git
    $ cd bashp
    $ make

### Usage

    $ ./bashp -c 'echo hello >world' | jq
    {
      "type": "simple",
      "flags": 0,
      "line": 0,
      "words": [
        "echo",
        "hello"
      ],
      "redirects": [
        {
          "redirector": 1,
          "redirectee": "world",
          "rflags": 0,
          "flags": 577,
          "instruction": 0,
          "here_doc_eof": null
        }
      ]
    }
