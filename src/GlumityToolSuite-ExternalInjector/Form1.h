#pragma once
#include "Injector.h"

namespace CppCLRWinFormsProject
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	Injector injector;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TableLayoutPanel^ tableLayoutPanel1;
	private: System::Windows::Forms::Button^ button_inject;
	private: System::Windows::Forms::ListBox^ listBox_processes;
	private: System::Windows::Forms::Button^ button_refreshProcesses;
	private: System::Windows::Forms::TableLayoutPanel^ tableLayoutPanel2;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::TextBox^ textBox_filterByName;
	private: System::Windows::Forms::Button^ button_resetFilter;
	private: System::Windows::Forms::TableLayoutPanel^ tableLayoutPanel3;

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->button_resetFilter = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox_filterByName = (gcnew System::Windows::Forms::TextBox());
			this->listBox_processes = (gcnew System::Windows::Forms::ListBox());
			this->button_refreshProcesses = (gcnew System::Windows::Forms::Button());
			this->tableLayoutPanel3 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->button_inject = (gcnew System::Windows::Forms::Button());
			this->tableLayoutPanel1->SuspendLayout();
			this->tableLayoutPanel2->SuspendLayout();
			this->tableLayoutPanel3->SuspendLayout();
			this->SuspendLayout();
			//
			// tableLayoutPanel1
			//
			this->tableLayoutPanel1->ColumnCount = 2;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				80.88235F)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				19.11765F)));
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanel2, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->listBox_processes, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->button_refreshProcesses, 1, 1);
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanel3, 1, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 2;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10.07463F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 89.92538F)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(1020, 536);
			this->tableLayoutPanel1->TabIndex = 0;
			//
			// tableLayoutPanel2
			//
			this->tableLayoutPanel2->ColumnCount = 2;
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				138)));
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				20)));
			this->tableLayoutPanel2->Controls->Add(this->button_resetFilter, 0, 1);
			this->tableLayoutPanel2->Controls->Add(this->label1, 0, 0);
			this->tableLayoutPanel2->Controls->Add(this->textBox_filterByName, 0, 1);
			this->tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel2->Location = System::Drawing::Point(3, 3);
			this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
			this->tableLayoutPanel2->RowCount = 2;
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 43.75F)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 56.25F)));
			this->tableLayoutPanel2->Size = System::Drawing::Size(818, 48);
			this->tableLayoutPanel2->TabIndex = 3;
			//
			// button_resetFilter
			//
			this->button_resetFilter->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->button_resetFilter->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->button_resetFilter->ForeColor = System::Drawing::SystemColors::Control;
			this->button_resetFilter->Location = System::Drawing::Point(683, 24);
			this->button_resetFilter->Name = L"button_resetFilter";
			this->button_resetFilter->Size = System::Drawing::Size(132, 21);
			this->button_resetFilter->TabIndex = 2;
			this->button_resetFilter->Text = L"Reset Filter";
			this->button_resetFilter->UseVisualStyleBackColor = true;
			this->button_resetFilter->Click += gcnew System::EventHandler(this, &Form1::button_resetFilter_Click);
			//
			// label1
			//
			this->label1->AutoSize = true;
			this->label1->Dock = System::Windows::Forms::DockStyle::Left;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::SystemColors::Control;
			this->label1->Location = System::Drawing::Point(3, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(213, 21);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Filter process by name";
			//
			// textBox_filterByName
			//
			this->textBox_filterByName->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(60)),
				static_cast<System::Int32>(static_cast<System::Byte>(60)), static_cast<System::Int32>(static_cast<System::Byte>(60)));
			this->textBox_filterByName->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox_filterByName->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->textBox_filterByName->ForeColor = System::Drawing::SystemColors::Control;
			this->textBox_filterByName->Location = System::Drawing::Point(3, 25);
			this->textBox_filterByName->Name = L"textBox_filterByName";
			this->textBox_filterByName->Size = System::Drawing::Size(674, 13);
			this->textBox_filterByName->TabIndex = 1;
			this->textBox_filterByName->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_filterByName_TextChanged);
			//
			// listBox_processes
			//
			this->listBox_processes->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(40)),
				static_cast<System::Int32>(static_cast<System::Byte>(40)), static_cast<System::Int32>(static_cast<System::Byte>(40)));
			this->listBox_processes->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->listBox_processes->Dock = System::Windows::Forms::DockStyle::Fill;
			this->listBox_processes->ForeColor = System::Drawing::SystemColors::Control;
			this->listBox_processes->FormattingEnabled = true;
			this->listBox_processes->Location = System::Drawing::Point(3, 57);
			this->listBox_processes->Name = L"listBox_processes";
			this->listBox_processes->Size = System::Drawing::Size(818, 476);
			this->listBox_processes->TabIndex = 1;
			//
			// button_refreshProcesses
			//
			this->button_refreshProcesses->Dock = System::Windows::Forms::DockStyle::Fill;
			this->button_refreshProcesses->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->button_refreshProcesses->ForeColor = System::Drawing::SystemColors::Control;
			this->button_refreshProcesses->Location = System::Drawing::Point(827, 57);
			this->button_refreshProcesses->Name = L"button_refreshProcesses";
			this->button_refreshProcesses->Size = System::Drawing::Size(190, 476);
			this->button_refreshProcesses->TabIndex = 2;
			this->button_refreshProcesses->Text = L"Refresh list";
			this->button_refreshProcesses->UseVisualStyleBackColor = true;
			this->button_refreshProcesses->Click += gcnew System::EventHandler(this, &Form1::button_refreshProcesses_Click);
			//
			// tableLayoutPanel3
			//
			this->tableLayoutPanel3->ColumnCount = 1;
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel3->Controls->Add(this->button_inject, 0, 1);
			this->tableLayoutPanel3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel3->Location = System::Drawing::Point(827, 3);
			this->tableLayoutPanel3->Name = L"tableLayoutPanel3";
			this->tableLayoutPanel3->RowCount = 2;
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 43.75F)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 56.25F)));
			this->tableLayoutPanel3->Size = System::Drawing::Size(190, 48);
			this->tableLayoutPanel3->TabIndex = 4;
			//
			// button_inject
			//
			this->button_inject->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->button_inject->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->button_inject->ForeColor = System::Drawing::SystemColors::Control;
			this->button_inject->Location = System::Drawing::Point(3, 24);
			this->button_inject->Name = L"button_inject";
			this->button_inject->Size = System::Drawing::Size(184, 21);
			this->button_inject->TabIndex = 0;
			this->button_inject->Text = L"Inject";
			this->button_inject->UseVisualStyleBackColor = true;
			this->button_inject->Click += gcnew System::EventHandler(this, &Form1::button_inject_Click);
			//
			// Form1
			//
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(40)), static_cast<System::Int32>(static_cast<System::Byte>(40)),
				static_cast<System::Int32>(static_cast<System::Byte>(40)));
			this->ClientSize = System::Drawing::Size(1020, 536);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Name = L"Form1";
			this->Text = L"GlummyLoaderInjector";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel2->ResumeLayout(false);
			this->tableLayoutPanel2->PerformLayout();
			this->tableLayoutPanel3->ResumeLayout(false);
			this->ResumeLayout(false);
		}
#pragma endregion
	private: System::Void button_inject_Click(System::Object^ sender, System::EventArgs^ e)
	{
		auto selectedProcess = (String^)listBox_processes->SelectedItem;

		String^ name = selectedProcess->Split('|')[0];
		injector.Inject(name->Remove(name->Length - 1, 1));
	}
	private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e)
	{
		injector.PopulateListWithProcesses(listBox_processes);
	}
	private: System::Void button_refreshProcesses_Click(System::Object^ sender, System::EventArgs^ e)
	{
		injector.PopulateListWithProcesses(listBox_processes);
	}

	private: System::Void textBox_filterByName_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		if (textBox_filterByName->Text == String::Empty)
		{
			injector.PopulateListWithProcesses(listBox_processes);
			return;
		}

		for (size_t i = 0; i < listBox_processes->Items->Count; i++)
		{
			String^ currentName = (String^)listBox_processes->Items[i];
			if (!currentName->ToLower()->Contains(textBox_filterByName->Text->ToLower()))
			{
				listBox_processes->Items->RemoveAt(i);
			}
		}
	}
	private: System::Void button_resetFilter_Click(System::Object^ sender, System::EventArgs^ e)
	{
		textBox_filterByName->Text = String::Empty;
	}
	};
}
