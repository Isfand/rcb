#include <string>
#include <vector>
#include <print>

#include "delete.hxx"
#include "delete_args.hxx"
#include "common/globals.hxx"

namespace rcb{

DeleteArgs::DeleteArgs(std::vector<std::string>& args)
{
	run(args);
}

void DeleteArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	for(const auto& it : args)
	{
		std::println("In delete_args:{}", it);
	}
#endif

	if(args.empty())
	{
		std::println("{0} delete: missing operand\ntry \'{0} --help\' for more information", g_kProgName);
		return;
	}

	for(auto i { 0UZ }; i < args.size(); i++)
	{
	
		if(args[i].starts_with("--") ||  args[i].starts_with("-"))
		{
			// Add Conditional check for '-'. Reads stdin/stdout. E.g cat -

			if(args[i] == "--help" || args[i] == "-h")
			{
				std::print("{}", m_deleteHelpMsg);
				return;
			} 
			else if(args[i] == "--")
			{
				// Remove -- before passing arguments to delete cmd.
				m_erasePositions.push_back(i);
				break;
			} 
			else if(args[i] == "--verbose" || args[i] == "-v")
			{
				m_dOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			} 
			else if(args[i] == "--force" || args[i] == "-f")
			{
				m_dOpt.forceOption = true;
				m_erasePositions.push_back(i);
			}
			else if(args[i] == "--no-directorysize")
			{
				m_dOpt.noDirSizeOption = true;
				m_erasePositions.push_back(i);
			}
			else if(args[i] == "--dry-run")
			{
				m_dOpt.dryRunOption = true;
				m_erasePositions.push_back(i);
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

	// Needs to be removed in reverse to prevent index shift back.
	for (auto it = m_erasePositions.rbegin(); it != m_erasePositions.rend(); ++it) 
	{
		args.erase(args.begin() + *it);
	}

	Delete{args, m_dOpt, EnvResolver{}.resolve()};
}

} // namespace rcb
