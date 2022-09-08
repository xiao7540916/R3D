#include"r_log.h"
#include<spdlog/sinks/stdout_color_sinks.h>

namespace R3D {
	std::shared_ptr<spdlog::logger> RLog::m_logger;
	void RLog::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");   //�Զ�����־�������ʽ

		m_logger = spdlog::stdout_color_mt("R3D");//��־����  
		m_logger->set_level(spdlog::level::trace);//������־�����
	}
}