obj-$(CONFIG_PCIE_DW) += pcie-designware.o
obj-$(CONFIG_PCIE_DW_EP) += pcie-designware-ep.o
obj-$(CONFIG_PCIE_DW_PLAT) += pcie-designware-plat.o
ifneq ($(filter y, $(CONFIG_PCI_AR91XX_HOST) $(CONFIG_PCI_AR91XX_EP)),)
	obj-$(CONFIG_PCI_AR91XX) += pci-artosyn.o
endif
