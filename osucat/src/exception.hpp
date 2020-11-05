#pragma once
#ifndef OC_EXCEPTION_HPP
#define OC_EXCEPTION_HPP

namespace osucat {
    class exception {
    public:
        exception(const std::string& info, int code) {
            this->info = info;
            this->code = code;
        }
        int Code() {
            return this->code;
        }
        std::string Info() {
            return this->info;
        }
        virtual std::string Show() {
            time_t tt = time(NULL);
            tm _tm = { 0 };
            localtime_s(&_tm, &tt);
            char time_str[128];
            strftime(time_str, 128, "%Y-%m-%d %H:%M:%S", &_tm);
            char result[1024];
            sprintf_s(result, 1024, "[%s] %d - %s", time_str, this->code, this->info.c_str());
            return result;
        }
    private:
        std::string info;
        int32_t code;
        std::string message;
    };
    class NetWork_Exception : public exception {
    public:
        NetWork_Exception(const std::string& info, int code) : exception(info, code) {
        }
        std::string Show() {
            char buffer[512];
            sprintf_s(buffer,
                512,
                u8"ÍøÂç³öÏÖ´íÎó£¬´íÎó´úÂë£º%d£¬´íÎóÐÅÏ¢£º%s",
                this->Code(),
                this->Info().c_str());
            return buffer;
        }
    };
    class database_exception : public exception {
    public:
        database_exception(const std::string& info, int code) : exception(info, code) {
        }
        std::string Show() {
            char buffer[512];
            sprintf_s(buffer,
                512,
                u8"MySQL³öÏÖ´íÎó£¬´íÎó´úÂë£º%d£¬´íÎóÐÅÏ¢£º%s",
                this->Code(),
                this->Info().c_str());
            return buffer;
        }
    };
}

#endif