#pragma once

#include <memory>
#include <string>
#include <vector>
#include "IInterrupts.hh"
#include "coco/port/CpriConfiguration.hh"
#include "coco/port/portinterface/nokiacpri/IAggregateStates.hh"
#include "coco/utils/events/Dispatchers.hh"
#include "coco/utils/events/FileEventsDispatcher.hh"

namespace lola {
namespace cpri {
namespace states {

class ICpriStateMachineDriver;

} // namespace states
} // namespace cpri
} // namespace lola

namespace coco {
namespace port {
namespace portinterface {
namespace nokiacpri {
namespace interrupt {

std::unique_ptr<IInterrupts> createInterruptHandler(utils::events::Dispatchers&, IAggregateStates&,
                                                    lola::cpri::states::ICpriStateMachineDriver&, cpri::FpgaType,
                                                    std::vector<std::string>, bool isExternalLink);
