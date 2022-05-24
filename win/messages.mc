MessageIdTypedef = NTSTATUS

SeverityNames = (
	Success		= 0x0:STATUS_SEVERITY_SUCCESS
	Informational	= 0x1:STATUS_SEVERITY_INFORMATIONAL
	Warning		= 0x2:STATUS_SEVERITY_WARNING
	Error		= 0x3:STATUS_SEVERITY_ERROR
)

FacilityNames = (
	System		= 0x0
	Eventlog	= 0x2A:FACILITY_EVENTLOG_ERROR_CODE
)


MessageId	= 0x0001
Facility	= Eventlog
Severity	= Informational
SymbolicName	= VMLATENCY_LOG
Language	= English
%2
.
