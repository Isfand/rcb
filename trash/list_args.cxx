#include <print>
#include <vector>

#include "list_args.hxx"
#include "list.hxx"

namespace trash{

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
		if (std::string(args.at(i)).substr(0, 2) == "--" || std::string(args.at(i)).substr(0, 1) == "-")
		{
			if(std::string(args.at(i)) == "--help" || std::string(args.at(i)) == "-h")
			{
				std::print("{}", m_listHelpMsg);
					
				return;
			} 
			else if (std::string(args.at(i)) == "--all")
			{
#ifndef NDEBUG
				std::println("all triggered");
#endif
				m_lOpt.allOption = true;
				m_lOpt.totalSizeOption = true;
				m_lOpt.totalCountOption = true;
				m_lOpt.defaultOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--total-size")
			{
#ifndef NDEBUG
				std::println("total-size triggered");
#endif
				m_lOpt.totalSizeOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--total-count")
			{
#ifndef NDEBUG
				std::println("total-count triggered");
#endif
				m_lOpt.totalCountOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--human-readable")
			{
#ifndef NDEBUG
				std::println("human-readable triggered");
#endif
				m_lOpt.humanReadableOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--no-format")
			{
#ifndef NDEBUG
				std::println("no-format triggered");
#endif
				m_lOpt.noFormatOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--past")
			{
#ifndef NDEBUG
				std::println("present triggered");
#endif
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
			else if (std::string(args.at(i)) == "--previous")
			{
#ifndef NDEBUG
				std::println("previous triggered");
#endif
				m_lOpt.previousOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--verbose" || std::string(args.at(i)) == "-v")
			{
#ifndef NDEBUG
				std::println("verbose triggered");
#endif
				m_lOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--sql")
			{
#ifndef NDEBUG
				std::println("sql triggered");
#endif
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

} // namespace trash
