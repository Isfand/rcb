#include <print>
#include <vector>

#include "list_args.hxx"
#include "list.hxx"
#include "common/env.hxx"

namespace rcb{

ListArgs::ListArgs(std::vector<std::string>& args)
{
	run(args);
}

void ListArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	std::println("Inside List Args:");
	for(const auto& it : args)
	{
		std::println("{}", it);
	}
#endif

	for(auto i { 0UZ }; i < args.size(); i++)
	{
		if(args[i].starts_with("--") ||  args[i].starts_with("-"))
		{
			if(args[i] == "--help" || args[i] == "-h")
			{
				std::print("{}", m_listHelpMsg);
					
				return;
			} 
			else if (args[i] == "--all" || args[i] == "-a")
			{
				m_lOpt.allOption = true;
				m_lOpt.totalSizeOption = true;
				m_lOpt.totalCountOption = true;
				m_lOpt.defaultOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--total-size")
			{
				m_lOpt.totalSizeOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--total-count")
			{
				m_lOpt.totalCountOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--human-readable")
			{
				m_lOpt.humanReadableOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--no-format")
			{
				m_lOpt.noFormatOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--past")
			{
				if (i + 1UZ < args.size())
				{
					m_lOpt.pastOption = true;
					m_lOpt.timeVec.push_back(args.at(i + 1));
					m_erasePositions.push_back(i);
					m_erasePositions.push_back(i + 1);
				}
				else 
				{
					m_erasePositions.push_back(i);
					std::println("expected string value after index:{}", i);
					return;
				}
			}
			else if (args[i] == "--last")
			{
				m_lOpt.lastOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--verbose" || args[i] == "-v")
			{
				m_lOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--sql")
			{
				if (i + 1 < args.size())
				{
					m_lOpt.sqlOption = true;
					m_lOpt.sqlVec.push_back(args.at(i + 1));
					m_erasePositions.push_back(i);
					m_erasePositions.push_back(i + 1);
				}
				else 
				{
					m_erasePositions.push_back(i);
					std::println("expected string value after index:{}", i);
					return;
				}
			}
			else
			{
				std::println("unknown option: {0}", args[i]);
				return; // Prevent continuation
			}
		}
		else
		{
			// Unused
		}
	}
#ifndef NDEBUG
	std::println("Args to remove in list args:");
	for (auto it = m_erasePositions.rbegin(); it != m_erasePositions.rend(); ++it) 
	{
		std::println("{}", *it);
	}
#endif

	// Needs to be removed in reverse to prevent index shift back.
	for (auto it = m_erasePositions.rbegin(); it != m_erasePositions.rend(); ++it) 
	{
		args.erase(args.begin() + *it);
	}

	List{m_lOpt, EnvResolver{}.resolve()};
}

} // namespace rcb
