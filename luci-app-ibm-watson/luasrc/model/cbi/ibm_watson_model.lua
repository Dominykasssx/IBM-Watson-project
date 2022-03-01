map = Map("ibm-watson")

section = map:section(NamedSection, "config", "ibm_watson", "IBM Watson configuration")

flag = section:option(Flag, "enable", "Enable", "Enable program")

orgId = section:option( Value, "orgId", "Organization ID")
typeId = section:option( Value, "typeId", "Type ID")
deviceId = section:option( Value, "deviceId", "Device ID")
token = section:option( Value, "token", "Token")
token.datatype = "string"
token.maxlength = 32

return map
