#pragma once

#include "command.h"

namespace gmredis::command {
    class BaseCommand : public ::Command {
    public:
        std::expected<protocol::RespValue, CommandError> validate(const RespArray& arg) final override;
        std::expected<protocol::RespValue, CommandError> execute(const RespArray& arg) final override;

    protected:
        virtual std::expected<protocol::RespValue, CommandError> doValidate(const RespArray& arg) = 0;
        virtual std::expected<protocol::RespValue, CommandError> doExecute(const RespArray& arg) = 0;

        virtual void preValidate(const RespArray& arg) {};
        virtual void postValidate(const RespArray& arg, const RespValue& result) {};
        virtual void preExecute(const RespArray& arg) {};
        virtual void postExecute(const RespArray& arg, const RespValue& result) {};
        virtual void onError() {}
   }
}
