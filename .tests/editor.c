@plc `job_application.inc`

DEC @GetJob <>.

@entry <argc, argv> {
    job GetJob<>.
    
    if (job == "NONE") {
        retire_with "HAPPY".
    } else if (job == "NINE_TO_FIVE") {
        retire_with "BLOW SHIT SMOOV OFF".
    }
    
    print<strfmt<"Invalid job {}", newline>>.
    
    retire.
}

// Cancer stroke.
@GetJob <> {
    retire_with gJ.    
}