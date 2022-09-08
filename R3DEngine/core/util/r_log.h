#include<spdlog/spdlog.h>
namespace R3D {
    class RLog {
    public:
        static void Init();
        inline static std::shared_ptr<spdlog::logger> &GetLogger() {return m_logger;}
    private:
        static int age;
        static std::shared_ptr<spdlog::logger> m_logger;
    };
}