var md_doc_requirements =
[
    [ "1 System requirements", "sys.html", [
      [ "1.1 Prioritize project independence to hardware", "sys.html#autotoc_md6", [
        [ "1.1.1 Decomposition", "sys.html#autotoc_md7", null ],
        [ "1.1.2 Code shall not be dependent on hardware specifics unless necessary", "sys.html#autotoc_md8", null ]
      ] ],
      [ "1.2 Focus on testing", "sys.html#autotoc_md9", null ],
      [ "1.2.1 Test on host what can be tested on host", "sys.html#autotoc_md10", null ],
      [ "1.2.3 Use testing on target hardware", "sys.html#autotoc_md11", null ]
    ] ],
    [ "2 System control requirements", "ctl.html", [
      [ "2.1 Position control mode", "ctl.html#autotoc_md12", null ],
      [ "2.2 Current control mode", "ctl.html#autotoc_md13", null ],
      [ "2.3 Velocity control mode", "ctl.html#autotoc_md14", null ],
      [ "2.4 Power control mode", "ctl.html#autotoc_md15", null ],
      [ "2.5 Movement detection", "ctl.html#autotoc_md16", null ],
      [ "2.6 Static friction compensation", "ctl.html#autotoc_md17", null ],
      [ "2.7 Disengaged", "ctl.html#autotoc_md18", null ]
    ] ],
    [ "3 System limits requirements", "lims.html", [
      [ "3.1 Current limits", "lims.html#autotoc_md19", null ],
      [ "3.2 Position limits", "lims.html#autotoc_md20", null ],
      [ "3.3 Velocity limits", "lims.html#autotoc_md21", null ]
    ] ],
    [ "4 Communication requirements", "comms.html", [
      [ "4.1 System should have one major communication interface", "comms.html#autotoc_md22", null ],
      [ "4.2 System should decompose message communication to be independent on underlying transport layer", "comms.html#autotoc_md23", null ],
      [ "4.3 UART shall be supported as transport", "comms.html#autotoc_md24", null ],
      [ "4.4 Major communication message shall be defined in one protobuf file", "comms.html#autotoc_md25", null ],
      [ "4.5 COBS can be used as framing mechanism", "comms.html#autotoc_md26", null ],
      [ "4.6 Exact communication interface is property of board", "comms.html#autotoc_md27", null ],
      [ "4.7 ID", "comms.html#autotoc_md28", null ],
      [ "4.8 Group ID", "comms.html#autotoc_md29", null ]
    ] ],
    [ "5 Units", "units.html", [
      [ "5.1 Any values facing the user shall be in well defined units specified by other requirements", "units.html#autotoc_md30", null ],
      [ "5.2 Angular position is specified in radians", "units.html#autotoc_md31", null ],
      [ "5.3 Electric current is specified in amperes", "units.html#autotoc_md32", null ],
      [ "5.4 Time is specified in seconds", "units.html#autotoc_md33", null ],
      [ "5.5 Angular velocity is specified in radians per second.", "units.html#autotoc_md34", null ]
    ] ],
    [ "6 Configuration", "config.html", [
      [ "6.1 One configuration", "config.html#autotoc_md35", null ],
      [ "6.2 Configuration can be managed by the user via the communication interface", "config.html#autotoc_md36", [
        [ "6.2.1 Get configuration field", "config.html#autotoc_md37", null ],
        [ "6.2.2 Set configuration field", "config.html#autotoc_md38", null ],
        [ "6.2.3 Commit the configuration", "config.html#autotoc_md39", null ],
        [ "6.2.4 Clear the configuration", "config.html#autotoc_md40", null ]
      ] ],
      [ "6.3 Configuration shall be documented", "config.html#autotoc_md41", null ],
      [ "6.4 Configuration is tied to the version of firmware", "config.html#autotoc_md42", null ],
      [ "6.5 Persistent configuration", "config.html#autotoc_md43", [
        [ "6.5.1 Entire set is always stored", "config.html#autotoc_md44", null ],
        [ "6.5.2 Size of history", "config.html#autotoc_md45", null ],
        [ "6.5.3 Clear removes history", "config.html#autotoc_md46", null ],
        [ "6.5.4 Firmware version is also stored", "config.html#autotoc_md47", null ]
      ] ],
      [ "6.6 Firmware version changes", "config.html#autotoc_md48", null ],
      [ "6.7 Utility for changes shall exist", "config.html#autotoc_md49", null ],
      [ "6.8 Common format for storing on host side", "config.html#autotoc_md50", null ],
      [ "6.9 List of fields for configuration", "config.html#autotoc_md51", [
        [ "6.9.1 Model", "config.html#autotoc_md52", null ],
        [ "6.9.2 ID", "config.html#autotoc_md53", null ]
      ] ]
    ] ]
];