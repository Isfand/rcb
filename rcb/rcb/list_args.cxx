#include <print>
#include <vector>

#include "list_args.hxx"
#include "list.hxx"

namespace rcb{

ListArgs::ListArgs(std::vector<std::string>& args)
{
	run(args);
}

void ListArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	std::println("Inside List Args:");
	for(auto it : args)
	{
		std::println("{}", it);
	}
#endif

	for(auto i { 0UL }; i < args.size(); i++)
	{
		if (args.at(i).substr(0, 2) == "--" || args.at(i).substr(0, 1) == "-")
		{
			if(args.at(i) == "--help" || args.at(i) == "-h")
			{
				std::print("{}", m_listHelpMsg);
					
				return;
			} 
			else if (args.at(i) == "--all" || args.at(i) == "-a")
			{
				m_lOpt.allOption = true;
				m_lOpt.totalSizeOption = true;
				m_lOpt.totalCountOption = true;
				m_lOpt.defaultOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--total-size")
			{
				m_lOpt.totalSizeOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--total-count")
			{
				m_lOpt.totalCountOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--human-readable")
			{
				m_lOpt.humanReadableOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--no-format")
			{
				m_lOpt.noFormatOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--past")
			{
				if (i + 1UL < args.size())
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
			else if (args.at(i) == "--previous")
			{
				m_lOpt.previousOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--verbose" || args.at(i) == "-v")
			{
				m_lOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--sql")
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
				std::println("unknown option: {0}", args.at(i));
				return; //Prevent continuation
			}
		}
		else
		{
			//Unused
		}
	}
#ifndef NDEBUG
	std::println("Args to remove in list args:");
	for (auto it = m_erasePositions.rbegin(); it != m_erasePositions.rend(); ++it) 
	{
		std::println("{}", *it);
	}
#endif

	//Needs to be removed in reverse to prevent index shift back.
	for (auto it = m_erasePositions.rbegin(); it != m_erasePositions.rend(); ++it) 
	{
		args.erase(args.begin() + *it);
	}

	List{m_lOpt};
}

} // namespace rcb
