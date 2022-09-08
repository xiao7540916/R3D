#include"r_log.h"
#include<spdlog/sinks/stdout_color_sinks.h>

namespace R3D {
	std::shared_ptr<spdlog::logger> RLog::m_logger;
	void RLog::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");   //自定义日志的输出格式

		m_logger = spdlog::stdout_color_mt("R3D");//日志对象  
		m_logger->set_level(spdlog::level::trace);//所有日志均输出
	}
}