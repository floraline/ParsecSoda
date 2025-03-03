#include "GamepadsWidget.h"

GamepadsWidget::GamepadsWidget(Hosting& hosting)
    : _hosting(hosting), _gamepads(hosting.getGamepads())
{
}

bool GamepadsWidget::render()
{
    static ImVec2 dummySize = ImVec2(0.0f, 5.0f);

    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 380), ImVec2(800, 900));
    ImGui::Begin("Gamepads", (bool*)0);
    AppStyle::pushInput();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();

    static float indentDistance;

    static vector<Gamepad>::iterator gi;
    gi = _gamepads.begin();
    static int index;
    index = 0;

    for (; gi != _gamepads.end(); ++gi)
    {
        static uint32_t userID;
        userID = (*gi).owner.guest.userID;

        ImGui::BeginChild(
            (string("##Gamepad " ) + to_string(index)).c_str(),
            ImVec2(size.x, 50)
        );

        static ImVec2 cursor;
        cursor = ImGui::GetCursorPos();
        
        static int padIndex = 0;
        padIndex = (int)(*gi).getIndex() + 1;
        static bool isIndexFailure = false;
        isIndexFailure = padIndex <= 0 && (*gi).isConnected();

        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));
        ImGui::SetNextItemWidth(40.0f);
        if (isIndexFailure) AppColors::pushWarning();
        else AppColors::pushInput();
        AppFonts::pushTitle();
        if (ImGui::DragInt(
            (string("##GamepadIndex") + to_string(index)).c_str(),
            &padIndex, 0.1f, 0, 4
        ));
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        AppColors::pop();
        AppFonts::pop();
        ImGui::EndGroup();
        (*gi).setIndex((ULONG)(padIndex - 1));
        if (isIndexFailure)
        {
            TitleTooltipWidget::render(
                "XInput index",
                (
                    string("Windows failed to inform the correct index.\n") +
                    string("You have to find it manually by trial and error.")
                ).c_str()
            );
        }
        else
        {
            TitleTooltipWidget::render(
                "XInput index",
                (
                    string("Sometimes XInput fails to retrieve the index.\n") +
                    string("It should be a number in range [1, 4].")
                ).c_str()
            );
        }
        

        ImGui::SameLine();

        if (IconButton::render(AppIcons::back, AppColors::primary))
        {
            (*gi).clearOwner();
        }
        TitleTooltipWidget::render("Strip gamepad", "Unlink current user from this gamepad.");

        ImGui::SameLine();

        static ImVec4 colorOn;
        colorOn = padIndex > 0 ? AppColors::positive : AppColors::warning;
        if (ToggleIconButtonWidget::render(AppIcons::padOn, AppIcons::padOff, (*gi).isConnected(), colorOn))
        {
            if ((*gi).isConnected())
                (*gi).disconnect();
            else
                (*gi).connect();
        }
        if ((*gi).isConnected()) TitleTooltipWidget::render("Connected gamepad", "Press to \"physically\" disconnect\nthis gamepad (at O.S. level).");
        else                     TitleTooltipWidget::render("Disconnected gamepad", "Press to \"physically\" connect\nthis gamepad (at O.S. level).");

        ImGui::SameLine();
        
        static float gamepadLabelWidth;
        gamepadLabelWidth = size.x - 180.0f;
        
        ImGui::BeginChild(
            (string("##name ") + to_string(index)).c_str(),
            ImVec2(gamepadLabelWidth, 50.0f)
        );
        cursor = ImGui::GetCursorPos();

        ImGui::Dummy(ImVec2(0,8));

        AppStyle::pushLabel();
        if ((*gi).owner.guest.isValid())  ImGui::TextWrapped("(# %d)\t", (*gi).owner.guest.userID);
        else                              ImGui::TextWrapped("    ");
        AppStyle::pop();

        AppFonts::pushInput();
        AppColors::pushPrimary();
        ImGui::SetNextItemWidth(gamepadLabelWidth);
        ImGui::Text((*gi).owner.guest.name.c_str());
        AppColors::pop();
        AppFonts::pop();

        static ImVec2 backupCursor;
        backupCursor = ImGui::GetCursorPos();

        ImGui::SetCursorPos(cursor);
        ImGui::Button(
            (string("##gamepad button") + to_string(index + 1)).c_str(),
            ImVec2(gamepadLabelWidth, 50.0f)
        );

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("Gamepad", &index, sizeof(int));

            AppFonts::pushInput();
            AppColors::pushPrimary();
            ImGui::Text("%s", ((*gi).owner.guest.isValid() ? (*gi).owner.guest.name.c_str() : "Empty gamepad"));
            AppColors::pop();
            AppFonts::pop();

            AppStyle::pushLabel();
            ImGui::Text("Drop into another Gamepad to swap.");
            AppStyle::pop();

            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Guest"))
            {
                if (payload->DataSize == sizeof(int))
                {
                    int guestIndex = *(const int*)payload->Data;
                    if (guestIndex >= 0 && guestIndex < _hosting.getGuestList().size())
                    {
                        (*gi).owner.guest.copy(_hosting.getGuestList()[guestIndex]);
                    }
                }
            }
            else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Gamepad"))
            {
                if (payload->DataSize == sizeof(int))
                {
                    int sourceIndex = *(const int*)payload->Data;
                    if (sourceIndex >= 0 && sourceIndex < _gamepads.size())
                    {
                        static GuestDevice backupOwner;
                        backupOwner.copy(_gamepads[index].owner);
                        
                        _gamepads[index].copyOwner(_gamepads[sourceIndex]);
                        _gamepads[sourceIndex].owner.copy(backupOwner);
                        _gamepads[index].clearState();
                        _gamepads[sourceIndex].clearState();
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::SetCursorPos(backupCursor);

        ImGui::EndChild();
        
        ImGui::SameLine();
        
        static int deviceIndices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));
        ImGui::SetNextItemWidth(40);
        deviceIndices[index] = (*gi).owner.deviceID;

        AppFonts::pushTitle();
        if (ImGui::DragInt(
            (string("##DeviceIndex") + to_string(index)).c_str(),
            &deviceIndices[index], 0.1f, -1, 65536
        ))
        {
            (*gi).owner.deviceID = deviceIndices[index];
        }
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        AppFonts::pop();
        TitleTooltipWidget::render("Device index", "A guest may have multiple gamepads in the same machine.");
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::EndChild();

        index++;
    }

    ImGui::Dummy(dummySize);

    indentDistance = 0.5f * size.x - 40.0f;
    ImGui::Indent(indentDistance);
    if (ToggleIconButtonWidget::render(
        AppIcons::lock, AppIcons::unlock, _hosting.isGamepadLock(),
        AppColors::negative, AppColors::positive, ImVec2(80, 80)
    ))
    {
        _hosting.toggleGamepadLock();
    }
    if (_hosting.isGamepadLock())   TitleTooltipWidget::render("Unlock guest inputs", "Guests will be able to control gamepads again.");
    else                            TitleTooltipWidget::render("Lock guest inputs", "Guest inputs will be locked out of gamepads.");
    ImGui::Unindent(indentDistance);


    static ImVec2 cursor;
    cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(15.0f, size.y + 10.0f));
    ImGui::BeginGroup();
    if (IconButton::render(AppIcons::refresh, AppColors::primary, ImVec2(30.0f, 30.0f)))
    {
        _hosting.getGamepadClient().resetAll();
    }
    TitleTooltipWidget::render("Reset gamepad engine", "If all else fails, try this button.\nPress in dire situations.");
    ImGui::SameLine();
    if (IconButton::render(AppIcons::sort, AppColors::primary, ImVec2(30.0f, 30.0f)))
    {
        _hosting.getGamepadClient().sortGamepads();
    }
    TitleTooltipWidget::render("Sort gamepads", "Re-sort all gamepads by index.");
    ImGui::EndGroup();
    ImGui::SetCursorPos(cursor);

    ImGui::PopStyleVar();

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}