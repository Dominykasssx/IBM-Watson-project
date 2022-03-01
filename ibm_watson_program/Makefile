include $(TOPDIR)/rules.mk

PKG_NAME:=ibm_watson_program
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/ibm_watson_program
	CATEGORY:=Base system
	TITLE:=ibm_watson_program
	DEPENDS:=+libopenssl +lib_ibm_watson +ubox
endef

define Package/ibm_watson_program/description
	This is an example app package
endef

define Package/ibm_watson_program/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d 
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ibm_watson_program $(1)/usr/bin
	$(INSTALL_BIN) ./files/ibm-watson.init $(1)/etc/init.d/ibm-watson
	$(INSTALL_CONF) ./files/ibm-watson.config $(1)/etc/config/ibm-watson
endef

$(eval $(call BuildPackage,ibm_watson_program))