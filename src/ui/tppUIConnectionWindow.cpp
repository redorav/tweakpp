#include "tppUIConnectionWindow.h"

#include "client/tppClientVariableManager.h"
#include "client/tppSaveData.h"
#include "ui/tppUILog.h"
#include "ui/tppUITextIcons.h"
#include "ui/tppUIWidgets.h"
#include "tppCommon.h"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"

#include <chrono>
#include <algorithm>

namespace tpp
{
	void UIConnectionWindow::ShowContextMenu(tpp::VariableBase* variable, tpp::UIInteractionData& interactionData)
	{
		tpp::Assert(variable != nullptr);
		tpp::Assert(m_selectedGroupNode != nullptr);
		tpp::Assert(m_selectedGroupNode->variableGroup != nullptr);

		bool isFavoriteGroup = m_selectedGroupNode->variableGroup->GetIsFavorite();

		if (ImGui::BeginPopupContextItem(variable->GetName().c_str(), ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Revert To Default"))
			{
				variable->RevertToDefault();
			}

			if (isFavoriteGroup)
			{
				if (ImGui::MenuItem("Remove from Favorites"))
				{
					interactionData.removedFromFavorites = variable;
					m_dirty = true;
				}
			}
			else
			{
				if (ImGui::MenuItem("Add to Favorites"))
				{
					interactionData.addedToFavorites = variable;
					m_dirty = true;
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Copy Name"))
			{
				ImGui::SetClipboardText(variable->GetName().c_str());
			}

			if (ImGui::MenuItem("Copy Group Path"))
			{
				ImGui::SetClipboardText(variable->GetGroupPath().c_str());
			}

			if (ImGui::MenuItem("Copy Full Path"))
			{
				ImGui::SetClipboardText(variable->GetPath().c_str());
			}

			ImGui::EndPopup();
		}
	}

	void UIConnectionWindow::ShowDisabledVariableContextMenu(const std::string& variablePath, tpp::UIInteractionData& interactionData)
	{
		if (ImGui::BeginPopupContextItem(variablePath.c_str(), ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Remove from Favorites"))
			{
				interactionData.removedFromFavoritePath = variablePath;
				m_dirty = true;
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Copy Full Path"))
			{
				ImGui::SetClipboardText(variablePath.c_str());
			}

			ImGui::EndPopup();
		}
	}

	void UIConnectionWindow::ShowTooltip(tpp::VariableBase* variable)
	{
		using namespace std::chrono;

		if (ImGui::IsItemHovered())
		{
			uint64_t currentTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

			if (!m_wasHovering)
			{
				m_lastStartedHover = currentTime;
				m_wasHovering = variable;
			}

			uint64_t timeDelta = currentTime - m_lastStartedHover;

			if (timeDelta > 1000)
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(variable->GetPath().c_str());

				if (variable->type != VariableType::Callback)
				{
					ImGui::Separator();
				}

				if (variable->type == tpp::VariableType::Float)
				{
					tpp::Float* floatVariable = static_cast<tpp::Float*>(variable);
					ImGui::Text("Default: %f", floatVariable->metadata.defaultValue);
					ImGui::Text("Min: %f", floatVariable->metadata.minValue);
					ImGui::Text("Max: %f", floatVariable->metadata.maxValue);
					ImGui::Text("Step: %f", floatVariable->metadata.step);
				}
				else if (variable->type == tpp::VariableType::UnsignedInteger)
				{
					tpp::UInt* uintVariable = static_cast<tpp::UInt*>(variable);
					ImGui::Text("Default: %i", uintVariable->metadata.defaultValue);
					ImGui::Text("Min: %i", uintVariable->metadata.minValue);
					ImGui::Text("Max: %i", uintVariable->metadata.maxValue);
					ImGui::Text("Step: %i", uintVariable->metadata.step);
				}
				else if (variable->type == tpp::VariableType::Integer)
				{
					tpp::Int* intVariable = static_cast<tpp::Int*>(variable);
					ImGui::Text("Default: %i", intVariable->metadata.defaultValue);
					ImGui::Text("Min: %i", intVariable->metadata.minValue);
					ImGui::Text("Max: %i", intVariable->metadata.maxValue);
					ImGui::Text("Step: %i", intVariable->metadata.step);
				}
				else if (variable->type == tpp::VariableType::Bool)
				{
					tpp::Bool* boolVariable = static_cast<tpp::Bool*>(variable);
					ImGui::Text("Default: %s", boolVariable->metadata.defaultValue ? "true" : "false");
				}
				else if (variable->type == tpp::VariableType::Color3)
				{
					tpp::Color3* color3Variable = static_cast<tpp::Color3*>(variable);
					ImGui::Text("Default: [%f, %f, %f]", color3Variable->metadata.r, color3Variable->metadata.g, color3Variable->metadata.b);
				}
				else if (variable->type == tpp::VariableType::Color4)
				{
					tpp::Color4* color3Variable = static_cast<tpp::Color4*>(variable);
					ImGui::Text("Default: [%f, %f, %f, %f]", color3Variable->metadata.r, color3Variable->metadata.g, color3Variable->metadata.b, color3Variable->metadata.a);
				}
				else if (variable->type == tpp::VariableType::Vector2)
				{
					tpp::Vector2* vector2Variable = static_cast<tpp::Vector2*>(variable);
					ImGui::Text("Default: [%f, %f]", vector2Variable->metadata.x, vector2Variable->metadata.y);
				}
				else if (variable->type == tpp::VariableType::Vector3)
				{
					tpp::Vector3* vector3Variable = static_cast<tpp::Vector3*>(variable);
					ImGui::Text("Default: [%f, %f, %f]", vector3Variable->metadata.x, vector3Variable->metadata.y, vector3Variable->metadata.z);
				}
				else if (variable->type == tpp::VariableType::Vector4)
				{
					tpp::Vector4* vector4Variable = static_cast<tpp::Vector4*>(variable);
					ImGui::Text("Default: [%f, %f, %f, %f]", vector4Variable->metadata.x, vector4Variable->metadata.y, vector4Variable->metadata.z, vector4Variable->metadata.w);
				}
				else if (variable->type == tpp::VariableType::String)
				{
					tpp::String* stringVariable = static_cast<tpp::String*>(variable);
					ImGui::Text("Default: %s", stringVariable->defaultValue.c_str());
				}
				else if (variable->type == tpp::VariableType::Enum)
				{
					tpp::Enum* enumVariable = static_cast<tpp::Enum*>(variable);
					const tpp::EnumEntry& defaultEnumEntry = enumVariable->metadata.entries[enumVariable->metadata.defaultValue];
					ImGui::Text("Default: %s", defaultEnumEntry.name.c_str());
				}

				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
		else
		{
			if (m_wasHovering == variable)
			{
				m_wasHovering = nullptr;
			}
		}
	}

	void UIConnectionWindow::UpdateCachedVariables(const tpp::ClientVariableManager* variableManager)
	{
		if (m_dirty)
		{
			m_selectedGroupNode = m_nextSelectedGroupNode;

			// Clear regardless of whether the group is null or not. If it is null, we don't show any variables
			m_cachedVariables.clear();

			if (m_selectedGroupNode && m_selectedGroupNode->variableGroup)
			{
				size_t variableCount = m_selectedGroupNode->variableGroup->variableStrings.size();
				m_cachedVariables.reserve(variableCount);

				variableManager->ForEachVariableInGroup(m_selectedGroupNode->variableGroup,
					[this](const std::string& variablePath, tpp::VariableBase* variable)
				{
					size_t lastSlashOffset = variablePath.find_last_of("/");

					if (lastSlashOffset == variablePath.npos)
					{
						lastSlashOffset = 0;
					}

					// Include null variables here and deal with them on render. That is so that we can render out their names
					// when they aren't present but the favorite group has a reference
					m_cachedVariables.push_back({ variable, variablePath });
				});

				struct
				{
					bool operator()(CachedVariable& a, CachedVariable& b) const
					{
						if (a.variable && b.variable)
						{
							return a.variable->GetName() > b.variable->GetName();
						}
						else
						{
							return a.variablePath > b.variablePath;
						}
					}
				} SortAscending;

				struct
				{
					bool operator()(CachedVariable& a, CachedVariable& b) const
					{
						if (a.variable && b.variable)
						{
							return a.variable->GetName() < b.variable->GetName();
						}
						else
						{
							return a.variablePath < b.variablePath;
						}
					}
				} SortDescending;

				switch (m_sortOrder)
				{
					case tpp::SortOrder::Descending: std::sort(m_cachedVariables.begin(), m_cachedVariables.end(), SortDescending); break;
					case tpp::SortOrder::Ascending: std::sort(m_cachedVariables.begin(), m_cachedVariables.end(), SortAscending); break;
					default: break;
				}
			}

			m_dirty = false;
		}
	}

	template<typename FlagsT>
	bool RenderFlagsWidget(FlagsT* variable)
	{
		ImGui::PushID(variable);

		const std::vector<std::string>& entries = variable->metadata.entries;
		FlagsT::UnderlyingType currentValue = variable->currentValue;

		ImGui::Separator();

		FlagsT::UnderlyingType newValue = 0;

		for (int n = 0; n < entries.size(); n++)
		{
			bool boolValue = ((currentValue >> n) & 1) != 0;
			if (ImGui::Checkbox(entries[n].c_str(), &boolValue))
			{}

			newValue |= boolValue << n;
		}

		bool wasModified = currentValue != newValue;

		variable->currentValue = newValue;

		ImGui::Separator();

		ImGui::PopID();

		return wasModified;
	}

	// The invisible name is there to get a unique id but not display it using imgui's default positioning. We render the names separately
	bool DrawVariableWidget(const std::string& invisibleName, tpp::VariableBase* variable)
	{
		bool wasModified = false;

		if (variable)
		{
			if (variable->type == tpp::VariableType::Float)
			{
				tpp::Float* floatVariable = static_cast<tpp::Float*>(variable);
				wasModified = ImGui::SliderFloat(invisibleName.c_str(), &floatVariable->currentValue, floatVariable->metadata.minValue, floatVariable->metadata.maxValue);
			}
			else if (variable->type == tpp::VariableType::UnsignedInteger)
			{
				tpp::UInt* uintVariable = static_cast<tpp::UInt*>(variable);
				wasModified = ImGui::SliderScalar(invisibleName.c_str(), ImGuiDataType_U32, &uintVariable->currentValue, &uintVariable->metadata.minValue, &uintVariable->metadata.maxValue);
			}
			else if (variable->type == tpp::VariableType::Integer)
			{
				tpp::Int* intVariable = static_cast<tpp::Int*>(variable);
				wasModified = ImGui::SliderScalar(invisibleName.c_str(), ImGuiDataType_S32, &intVariable->currentValue, &intVariable->metadata.minValue, &intVariable->metadata.maxValue);
			}
			else if (variable->type == tpp::VariableType::Bool)
			{
				wasModified = ImGui::Checkbox(invisibleName.c_str(), &static_cast<tpp::Bool*>(variable)->currentValue);
			}
			else if (variable->type == tpp::VariableType::Color3)
			{
				wasModified = ImGui::ColorEdit3(invisibleName.c_str(), &static_cast<tpp::Color3*>(variable)->r);
			}
			else if (variable->type == tpp::VariableType::Color4)
			{
				wasModified = ImGui::ColorEdit4(invisibleName.c_str(), &static_cast<tpp::Color4*>(variable)->r);
			}
			else if (variable->type == tpp::VariableType::Vector2)
			{
				wasModified = ImGui::InputFloat2(invisibleName.c_str(), &static_cast<tpp::Vector2*>(variable)->x, "%.3f");
			}
			else if (variable->type == tpp::VariableType::Vector3)
			{
				wasModified = ImGui::InputFloat3(invisibleName.c_str(), &static_cast<tpp::Vector3*>(variable)->x, "%.3f");
			}
			else if (variable->type == tpp::VariableType::Vector4)
			{
				wasModified = ImGui::InputFloat4(invisibleName.c_str(), &static_cast<tpp::Vector4*>(variable)->x, "%.3f");
			}
			else if (variable->type == tpp::VariableType::String)
			{
				wasModified = ImGui::InputText(invisibleName.c_str(), &static_cast<tpp::String*>(variable)->currentValue);
			}
			else if (variable->type == tpp::VariableType::Enum)
			{
				tpp::Enum* enumVariable = static_cast<tpp::Enum*>(variable);
				const std::vector<EnumEntry>& entries = enumVariable->metadata.entries;

				ImGuiComboFlags comboFlags = 0;
				if (ImGui::BeginCombo(invisibleName.c_str(), entries[enumVariable->currentValue].name.c_str(), comboFlags))
				{
					for (int n = 0; n < entries.size(); n++)
					{
						bool isSelected = enumVariable->currentValue == n;
						if (ImGui::Selectable(entries[n].name.c_str(), isSelected))
						{
							wasModified = !isSelected;
							enumVariable->currentValue = n;
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			else if (variable->type == tpp::VariableType::Callback)
			{
				// Use the original name here as we want to display it on top of the button
				wasModified = ImGui::Button(variable->GetName().c_str());
			}
			else if (variable->type == tpp::VariableType::Flags8)
			{
				wasModified = RenderFlagsWidget(static_cast<tpp::Flags8*>(variable));
			}
			else if (variable->type == tpp::VariableType::Flags16)
			{
				wasModified = RenderFlagsWidget(static_cast<tpp::Flags16*>(variable));
			}
			else if (variable->type == tpp::VariableType::Flags32)
			{
				wasModified = RenderFlagsWidget(static_cast<tpp::Flags32*>(variable));
			}
			else if (variable->type == tpp::VariableType::Flags64)
			{
				wasModified = RenderFlagsWidget(static_cast<tpp::Flags64*>(variable));
			}
			else
			{
				printf("Variable type not implemented\n");
			}
		}

		return wasModified;
	}

	UIConnectionWindow::UIConnectionWindow(const tpp::ClientVariableManager* variableManager)
	{
		m_sortOrder = tpp::SaveData::GlobalSettings.defaultSortOrder;

		if (m_sortOrder == tpp::SortOrder::Descending)
		{
			m_defaultVariableSortOrder = ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_PreferSortDescending;
		}
		else if (m_sortOrder == tpp::SortOrder::Ascending)
		{
			m_defaultVariableSortOrder = ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_PreferSortAscending;
		}
		else
		{
			m_defaultVariableSortOrder = 0;
		}

		m_variablesWindowID = std::string("Variables Window##") + std::to_string((uintptr_t)this);
		m_logWindowID = std::string("Log##") + std::to_string((uintptr_t)this);

		m_uiLog = std::make_unique<tpp::UILog>();
		
		m_currentDisplayString = tpp::icons::LargeRedCircle;
		m_currentDisplayString += " ";
		m_currentDisplayString += variableManager->GetDisplayString();
	}

	void UIConnectionWindow::Draw(const tpp::ClientVariableManager* variableManager, bool* open, UIInteractionData& interactionData)
	{
		UpdateCachedVariables(variableManager);

		bool isConnected = variableManager->IsConnected();

		if (m_isConnected != isConnected)
		{
			if (isConnected)
			{
				m_currentDisplayString = tpp::icons::LargeGreenCircle;
			}
			else
			{
				m_currentDisplayString = tpp::icons::LargeRedCircle;
			}

			m_currentDisplayString += " ";
			m_currentDisplayString += variableManager->GetDisplayString();

			m_isConnected = isConnected;
		}

		const float FontSize = ImGui::GetFontSize();

		ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_NoCollapse;

		// We create a dockspace in the connection window to contain two child windows: the variable group + variables and the log
		// The top window is split into two vertically via an invisible table. The windows need a unique id because we reference
		// them globally when docking via ImGui::DockBuilderDockWindow

		ImGui::Begin(m_currentDisplayString.c_str(), open, windowFlags);
		{
			//-----------------------
			// Render the address bar
			//-----------------------

			// Address. We can pick up this address and modify it based on the selection, or parse it and navigate through it
			ImGui::Text("Address:");
			ImGui::SameLine();
			ImGuiInputTextFlags addressBarFlags = 0;
			addressBarFlags |= ImGuiInputTextFlags_AutoSelectAll;
			//addressFlags |= ImGuiInputTextFlags_CharsDecimal;

			float windowWidth = ImGui::GetContentRegionAvail().x;
			float currentCursorPosX = ImGui::GetCursorPosX();
			ImGui::PushItemWidth(windowWidth);
			bool modifiedAddress = ImGui::InputText("##Address:", &m_currentAddress, addressBarFlags);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::SetCursorPosX(currentCursorPosX + windowWidth - FontSize - 10.0f);
			ImGui::TextDisabled(tpp::icons::MagnifyingGlass);

			if (modifiedAddress)
			{
				// If we modified the address and we found a matching group navigate to the appropriate place
				// Otherwise the selected group node will be null and we won't show anything
				m_nextSelectedGroupNode = variableManager->GetVariableGroupNode(m_currentAddress);
				m_dirty = true;
			}

			ImVec2 windowSize = ImGui::GetWindowContentRegionMax();

			ImGuiID mainDockspaceID = ImGui::GetID(variableManager->GetDisplayString());
			if (!ImGui::DockBuilderGetNode(mainDockspaceID))
			{
				ImGui::DockBuilderRemoveNode(mainDockspaceID); // Clear existing contents
				ImGui::DockBuilderAddNode(mainDockspaceID, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);
				ImGui::DockBuilderSetNodeSize(mainDockspaceID, windowSize);

				ImGuiID dockMainID = mainDockspaceID;
				ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.2f, nullptr, &dockMainID);

				ImGui::DockBuilderDockWindow(m_variablesWindowID.c_str(), dockMainID);
				ImGui::DockBuilderDockWindow(m_logWindowID.c_str(), dockBottom);

				ImGuiID ids[] = { dockMainID, dockBottom };

				for (uint32_t i = 0; i < sizeof(ids) / sizeof(ImGuiID); ++i)
				{
					ImGuiDockNode* node = ImGui::DockBuilderGetNode(ids[i]);
					node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
					node->LocalFlags |= ImGuiDockNodeFlags_NoDocking;
					node->LocalFlags |= ImGuiDockNodeFlags_NoSplit;
				}

				ImGui::DockBuilderFinish(mainDockspaceID);
			}

			ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGuiDockNodeFlags dockNodeFlags = 0;
			//dockNodeFlags |= ImGuiDockNodeFlags_NoSplit;
			//dockNodeFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
			ImGui::DockSpace(mainDockspaceID, ImVec2(0.0f, 0.0f), dockNodeFlags);
			ImGui::PopStyleColor(); // ImGuiCol_DockingEmptyBg
		}
		ImGui::End();

		// Remove the padding inside the window
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

		// Remove padding of the window
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin(m_variablesWindowID.c_str(), nullptr);
		ImGui::PopStyleVar();
		{
			ImGuiTableFlags containerTableFlags = 0;
			containerTableFlags |= ImGuiTableFlags_Resizable;
			containerTableFlags |= ImGuiTableFlags_BordersOuter;
			containerTableFlags |= ImGuiTableFlags_ScrollY;

			ImGuiTableFlags innerTableFlags = 0;
			innerTableFlags |= ImGuiTableFlags_Resizable;    // Make sure we can resize this table
			innerTableFlags |= ImGuiTableFlags_ScrollY;      // Have a scrollbar
			innerTableFlags |= ImGuiTableFlags_PadOuterX;    // Padding around the titles. Inner padding is implicit
			innerTableFlags |= ImGuiTableFlags_Sortable;     // Make sure we can get the sort specs
			innerTableFlags |= ImGuiTableFlags_SortTristate; // Be able to return no sorting

			// Remove the padding between cells for the beginning of the table
			// This is to render the outer table completely invisible, except for the border
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
			bool beginContainerTable = ImGui::BeginTable("##ContainerTable", 2, containerTableFlags);
			ImGui::PopStyleVar(); // ImGuiStyleVar_CellPadding

			if (beginContainerTable)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				if (ImGui::BeginTable("##table1", 1, innerTableFlags))
				{
					ImGui::TableSetupScrollFreeze(0, 1);

					// Set up header rows
					ImGui::TableSetupColumn("Variable Groups");
					ImGui::TableHeadersRow();

					// Exit header row
					ImGui::TableNextRow();

					// Show variable groups
					ImGui::TableSetColumnIndex(0);

					ImGuiTreeNodeFlags treeNodeFlagsBase = 0;
					treeNodeFlagsBase |= ImGuiTreeNodeFlags_OpenOnArrow; // We want to be able to select it without opening
					treeNodeFlagsBase |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
					treeNodeFlagsBase |= ImGuiTreeNodeFlags_SpanAvailWidth;

					// Favorite groups go first
					variableManager->ForEachFavoriteGroupNode
					(
						[this, treeNodeFlagsBase](const std::string& groupName, const VariableGroupNode& favoriteGroup)
						{
							ImGuiTreeNodeFlags treeNodeFlags = treeNodeFlagsBase;

							// Favorite groups don't have subgroups, all variables are displayed inline
							treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;

							if (m_selectedGroupNode == &favoriteGroup)
							{
								treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
							}

							// Take the address of the variable as the unique id so it remains consistent across frames
							bool nodeOpen = tpp::imgui::TreeNodeEx((void*)&favoriteGroup, tpp::icons::NormalStar, treeNodeFlags, groupName.c_str());

							if (ImGui::IsItemClicked())
							{
								m_nextSelectedGroupNode = &favoriteGroup;
								m_dirty = true;
							}

							return nodeOpen;
						},
						[](bool isOpen)
						{
							if (isOpen)
							{
								ImGui::TreePop();
							}
						}
					);

					ImGui::Separator();

					variableManager->ForEachVariableGroupNode
					(
						[this, treeNodeFlagsBase](const std::string& nodeName, const VariableGroupNode& variableGroupNode)
						{
							ImGuiTreeNodeFlags treeNodeFlags = treeNodeFlagsBase;

							if (variableGroupNode.nodes.empty())
							{
								treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
							}

							if (&variableGroupNode == m_selectedGroupNode)
							{
								treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
							}

							// Take the address of the variable as the unique id so it remains consistent across frames
							bool nodeOpen = tpp::imgui::TreeNodeEx((void*)&variableGroupNode, tpp::icons::FileCode, treeNodeFlags, nodeName.c_str());

							if (ImGui::IsItemClicked())
							{
								m_nextSelectedGroupNode = &variableGroupNode;
								m_dirty = true;
							}

							return nodeOpen;
						},
						[](bool isOpen)
						{
							if (isOpen)
							{
								ImGui::TreePop();
							}
						}
					);
					ImGui::EndTable();
				}

				ImGui::TableSetColumnIndex(1);

				if (ImGui::BeginTable("##UIVariableWindow", 2, innerTableFlags))
				{
					ImGui::TableSetupScrollFreeze(0, 1);

					// Set up header rows
					// We can only sort the name column
					ImGui::TableSetupColumn("Name", m_defaultVariableSortOrder);
					ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoSort);
					ImGui::TableHeadersRow();

					ImGuiTableSortSpecs* tableSortSpecs = ImGui::TableGetSortSpecs();
					if (tableSortSpecs && tableSortSpecs->SpecsDirty)
					{
						if (tableSortSpecs->Specs)
						{
							switch (tableSortSpecs->Specs->SortDirection)
							{
								case ImGuiSortDirection_Descending: m_sortOrder = tpp::SortOrder::Descending; break;
								case ImGuiSortDirection_Ascending: m_sortOrder = tpp::SortOrder::Ascending; break;
								default: m_sortOrder = tpp::SortOrder::None; break;
							}
						}
						else
						{
							m_sortOrder = tpp::SortOrder::None;
						}

						m_dirty = true;
						tableSortSpecs->SpecsDirty = false;
					}

					// Exit header row
					ImGui::TableNextRow();

					if (m_selectedGroupNode)
					{
						for(CachedVariable& cachedVariable : m_cachedVariables)
						{
							ImGui::TableNextRow();

							ImGui::TableSetColumnIndex(0);

							float fontSize = ImGui::GetFontSize();

							if (cachedVariable.variable && !cachedVariable.variable->HasDefaultValue())
							{
								ImGui::Text(" "); ImGui::SameLine(); ImGui::Text(tpp::icons::LargeRedSquare); ImGui::SameLine();
							}

							ImGui::SetCursorPosX(fontSize * 2.0f); // TODO Check different font sizes

							if (cachedVariable.variable)
							{
								ImGui::Text(cachedVariable.variable->GetName().c_str());
								ShowContextMenu(cachedVariable.variable, interactionData);
								ShowTooltip(cachedVariable.variable);

								ImGui::TableSetColumnIndex(1);

								m_scratchPatchedNames = "##";
								m_scratchPatchedNames += cachedVariable.variable->GetName();
								bool wasModified = DrawVariableWidget(m_scratchPatchedNames, cachedVariable.variable);

								if (wasModified)
								{
									interactionData.editedVariable = cachedVariable.variable;
								}
							}
							else
							{
								ImGui::TextDisabled(cachedVariable.variablePath.c_str());
								ShowDisabledVariableContextMenu(cachedVariable.variablePath, interactionData);
							}
						}

						// Copy path into the address bar
						m_currentAddress = m_selectedGroupNode->m_path.c_str();
					}

					ImGui::EndTable();
				}

				ImGui::EndTable();
			}
		}
		ImGui::End();

		ImGui::PopStyleVar();

		bool logOpen = true;
		m_uiLog->Draw(m_logWindowID.c_str(), &logOpen);
	}

	void UIConnectionWindow::Log(const char* format...)
	{
		m_uiLog->Log(format);
	}

	void UIConnectionWindow::Clear()
	{
		m_selectedGroupNode = nullptr;

		m_nextSelectedGroupNode = nullptr;

		m_wasHovering = nullptr;
	}
}