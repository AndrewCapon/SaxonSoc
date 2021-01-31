package saxon.board.radiona.ulx3s

import saxon._
import spinal.core._
import spinal.lib.bus.bmb.{BmbClintGenerator, BmbPlicGenerator}
import spinal.lib.com.uart.UartCtrlMemoryMappedConfig
import spinal.lib.generator._
import spinal.lib.io.{Gpio, InOutWrapper}
import spinal.lib.misc.plic.PlicMapping
import vexriscv.VexRiscvBmbGenerator
import vexriscv.plugin.CsrPlugin


class Ulx3sBareMetalSystem extends BmbApbVexRiscvGenerator{
  //Add components
  val ramA = BmbOnChipRamGenerator(0x80000000l)
  val uartA = Apb3UartGenerator(0x10000)
  val gpioA = Apb3GpioGenerator(0x00000)
  val noReset = Ulx3sNoResetGenerator()

  val plic = Apb3PlicGenerator(0xC00000)
  plic.priorityWidth.load(2)
  plic.mapping.load(PlicMapping.sifive)
  plic.addTarget(cpu.externalInterrupt)

  val machineTimer = Apb3MachineTimerGenerator(0x08000)
  cpu.setTimerInterrupt(machineTimer.interrupt)

  ramA.dataWidth.load(32)

  //Interconnect specification
  interconnect.addConnection(
    cpu.iBus -> List(ramA.ctrl),
    cpu.dBus -> List(ramA.ctrl, peripheralBridge.input)
  )
}


class Ulx3sBareMetal extends Generator{
  val clockCtrl = ClockDomainResetGenerator()
  clockCtrl.holdDuration.load(255)
  clockCtrl.powerOnReset.load(true)

  val clocking = add task new Area{
    val clk_25mhz = in Bool()

    clockCtrl.setInput(
      ClockDomain(
        clock = clk_25mhz,
        frequency = FixedFrequency(25 MHz),
        config = ClockDomainConfig(
          resetKind = ASYNC,
          resetActiveLevel = LOW
        )
      )
    )
  }

  val system = new Ulx3sBareMetalSystem
  system.onClockDomain(clockCtrl.outputClockDomain)

}

object Ulx3sBareMetalSystem{
  def default(g : Ulx3sBareMetalSystem, clockCtrl : ClockDomainResetGenerator) = g {
    import g._

    cpu.config.load(VexRiscvConfigs.minimalWithCsr)
    cpu.enableJtag(clockCtrl, clockCtrl)

    ramA.size.load(32 KiB)
    ramA.hexInit.load("software/standalone/blinkAndEcho/build/blinkAndEcho.hex")

    uartA.parameter load UartCtrlMemoryMappedConfig(
      baudrate = 115200,
      txFifoDepth = 1,
      rxFifoDepth = 1
    )

    gpioA.parameter load Gpio.Parameter(width = 8)

    g
  }
}


object Ulx3sBareMetal {
  //Function used to configure the SoC
  def default(g : Ulx3sBareMetal) = g{
    import g._
    Ulx3sBareMetalSystem.default(system, clockCtrl)
    //clockCtrl.resetSensitivity.load(ResetSensitivity.NONE)
    g
  }

  //Generate the SoC
  def main(args: Array[String]): Unit = {
    val report = SpinalRtlConfig.generateVerilog(InOutWrapper(default(new Ulx3sBareMetal()).toComponent()))
    BspGenerator("radiona/ulx3s/BareMetal", report.toplevel.generator, report.toplevel.generator.system.cpu.dBus)
  }
}

