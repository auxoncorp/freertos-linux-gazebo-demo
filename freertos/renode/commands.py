from Antmicro import Renode
import uuid

def mc_clear_modality_noint_vars():
    sysbus = self.Machine["sysbus"]
    var_startup_nonce_addr = sysbus.GetSymbolAddress("g_startup_nonce")
    sysbus.WriteDoubleWord(var_startup_nonce_addr, 0)

def mc_write_startup_nonce(nonce):
    print("Writing startup nonce = %d" % nonce)
    sysbus = self.Machine["sysbus"]
    var_startup_nonce_addr = sysbus.GetSymbolAddress("g_startup_nonce")
    sysbus.WriteDoubleWord(var_startup_nonce_addr, nonce)
