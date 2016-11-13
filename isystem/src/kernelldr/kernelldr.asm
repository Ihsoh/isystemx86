.ORG 0

.BIT16

.SET KernelLoaderSeg, 1000H
.SET KernelLoaderOff, 0H
.SET KernelLoaderAddr, `(KernelLoaderSeg << 4) + KernelLoaderOff`

.LABEL KernelLoaderHeader

.LABEL KernelLoaderHeaderTail
.DBS `1000H - (KernelLoaderHeaderTail - KernelLoaderHeader)`








.LABEL MemorySize_AX
.DW 0
.LABEL MemorySize_BX
.DW 0

.LABEL VESAInfo
.DBS 512

.LABEL KnlldrMsg0
.STRING 'Kernel loader initializing...'
.DB 0

.END
