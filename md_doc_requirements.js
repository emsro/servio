var md_doc_requirements =
[
    [ "System requirements", "sys.html", [
      [ "REQ1.1 Prioritize project independence to hardware", "sys.html#autotoc_md6", [
        [ "REQ1.1.1 Decomposition", "sys.html#autotoc_md7", null ],
        [ "REQ1.1.2 Code shall not be dependent on hardware specifics unless necessary", "sys.html#autotoc_md8", null ]
      ] ],
      [ "REQ1.2 Focus on testing", "sys.html#autotoc_md9", null ],
      [ "REQ1.2.1 Test on host what can be tested on host", "sys.html#autotoc_md10", null ],
      [ "REQ1.2.3 Use testing on target hardware", "sys.html#autotoc_md11", null ]
    ] ],
    [ "System control requirements", "ctl.html", [
      [ "REQ2.1 Position control mode", "ctl.html#autotoc_md12", null ],
      [ "REQ2.2 Current control mode", "ctl.html#autotoc_md13", null ],
      [ "REQ2.3 Velocity control mode", "ctl.html#autotoc_md14", null ],
      [ "REQ2.4 Power control mode", "ctl.html#autotoc_md15", null ],
      [ "REQ2.5 Movement detection", "ctl.html#autotoc_md16", null ],
      [ "REQ2.6 Static friction compensation", "ctl.html#autotoc_md17", null ],
      [ "REQ2.7 Disengaged", "ctl.html#autotoc_md18", null ]
    ] ],
    [ "System limits requirements", "lims.html", [
      [ "REQ3.1 Current limits", "lims.html#autotoc_md19", null ],
      [ "REQ3.2 Position limits", "lims.html#autotoc_md20", null ],
      [ "REQ3.3 Velocity limits", "lims.html#autotoc_md21", null ]
    ] ],
    [ "Communication requirements", "comms.html", [
      [ "REQ4.1 System should have one major communication interface", "comms.html#autotoc_md22", null ],
      [ "REQ4.2 System should decompose message communication to be independent on underlying transport layer", "comms.html#autotoc_md23", null ],
      [ "RE4.3 UART shall be supported as transport", "comms.html#autotoc_md24", null ],
      [ "REQ4.4 Major communication message shall be defined in TBD", "comms.html#autotoc_md25", null ],
      [ "REQ4.5 COBS can be used as framing mechanism", "comms.html#autotoc_md26", null ],
      [ "REQ4.6 Exact communication interface is property of board", "comms.html#autotoc_md27", null ],
      [ "REQ4.7 ID", "comms.html#autotoc_md28", null ],
      [ "REQ4.8 Group ID", "comms.html#autotoc_md29", null ]
    ] ],
    [ "Units", "units.html", [
      [ "REQ5.1 Any values facing the user shall be in well defined units specified by other requirements", "units.html#autotoc_md30", null ],
      [ "REQ5.2 Angular position is specified in radians", "units.html#autotoc_md31", null ],
      [ "REQ5.3 Electric current is specified in amperes", "units.html#autotoc_md32", null ],
      [ "REQ5.4 Time is specified in seconds", "units.html#autotoc_md33", null ],
      [ "REQ5.5 Angular velocity is specified in radians per second.", "units.html#autotoc_md34", null ]
    ] ],
    [ "Configuration", "config.html", [
      [ "REQ6.1 One configuration", "config.html#autotoc_md35", null ],
      [ "REQ6.2 Configuration can be managed by the user via the communication interface", "config.html#autotoc_md36", [
        [ "REQ6.2.1 Get configuration field", "config.html#autotoc_md37", null ],
        [ "REQ6.2.2 Set configuration field", "config.html#autotoc_md38", null ],
        [ "REQ6.2.3 Commit the configuration", "config.html#autotoc_md39", null ],
        [ "REQ6.2.4 Clear the configuration", "config.html#autotoc_md40", null ]
      ] ],
      [ "REQ6.3 Configuration shall be documented", "config.html#autotoc_md41", null ],
      [ "REQ6.4 Configuration is tied to the version of firmware", "config.html#autotoc_md42", null ],
      [ "REQ6.5 Persistent configuration", "config.html#autotoc_md43", [
        [ "REQ6.5.1 Entire set is always stored", "config.html#autotoc_md44", null ],
        [ "REQ6.5.2 Size of history", "config.html#autotoc_md45", null ],
        [ "REQ6.5.3 Clear removes history", "config.html#autotoc_md46", null ],
        [ "REQ6.5.4 Firmware version is also stored", "config.html#autotoc_md47", null ]
      ] ],
      [ "REQ6.6 Firmware version changes", "config.html#autotoc_md48", null ],
      [ "REQ6.7 Utility for changes shall exist", "config.html#autotoc_md49", null ],
      [ "REQ6.8 Common format for storing on host side", "config.html#autotoc_md50", null ],
      [ "REQ6.9 List of fields for configuration", "config.html#autotoc_md51", [
        [ "REQ6.9.1 Model", "config.html#autotoc_md52", null ],
        [ "REQ6.9.2 ID", "config.html#autotoc_md53", null ]
      ] ]
    ] ]
];