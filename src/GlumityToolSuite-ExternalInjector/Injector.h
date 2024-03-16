#pragma once

namespace CppCLRWinFormsProject
{
	class Injector
	{
	public:
		void PopulateListWithProcesses(System::Windows::Forms::ListBox^ list);

		void Inject(System::String^ processTarget);
	};
}
