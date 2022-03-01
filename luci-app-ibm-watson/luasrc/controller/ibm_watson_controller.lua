module("luci.controller.ibm_watson_controller", package.seeall)

function index()
	entry({"admin", "services", "ibm_config"}, cbi("ibm_watson_model"), _("IBM Watson Configuration"),105)
end
