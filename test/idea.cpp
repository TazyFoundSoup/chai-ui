// This file can be ignored
// It is just me putting ideas into 

#include <Chai/ChaiUI.h>

int main() {
    using namespace chai;

    Window app("Chai Project Tracker", {720, 500});
    StateBox state;
    state["projects"] = std::map<std::string, std::vector<std::string>>{};
    state["showInput"] = false;
    state["currentProject"] = "";

    Locator("new-task-form");

    // Input dialog for new tasks
    auto inputForm = [&]() {
        std::string buffer = "";

        return VBox({
            Text("Add task to: " + state["currentProject"].get<std::string>()).padding(6),
            InputField().placeholder("Task title...").on_change([&](auto text) {
                buffer = text;
            }),
            HBox({
                Button("Add").on_click([&] {
                    auto& projects = state["projects"].get<std::map<std::string, std::vector<std::string>>>();
                    auto& current = projects[state["currentProject"].get<std::string>()];
                    current.push_back(buffer);
                    state["projects"] = projects;
                    state["showInput"] = false;
                    Spawn(Toast("Task added!"));
                }),
                Button("Cancel").on_click([&] {
                    state["showInput"] = false;
                })
            }).spacing(8)
        }).padding(12).background(Color(255,255,255)).border_radius(8).shadow(8);
    };

    // Watch for form trigger
    state.watch("showInput", [&](bool show) {
        if (show) {
            Spawn(inputForm().locateAt("new-task-form"));
        } else {
            Spawn.removeLocator("new-task-form");
        }
    });

    // Project list view (reactively updated)
    auto projectList = state.watch("projects", [&](const auto& projects) {
        std::vector<Widget> views;

        for (const auto& [project, tasks] : projects) {
            std::vector<Widget> taskWidgets;
            for (const auto& task : tasks) {
                taskWidgets.push_back(
                    Text("- " + task).padding(4)
                );
            }

            taskWidgets.push_back(
                Button("Add Task").on_click([&, project]() {
                    state["currentProject"] = project;
                    state["showInput"] = true;
                }).padding(6)
            );

            views.push_back(
                VBox({
                    Text(project).bold().padding(6),
                    VBox(taskWidgets).spacing(4)
                }).background(Color(240, 240, 240)).padding(10).border_radius(6).spacing(6)
            );
        }

        return ScrollView(HBox(views).spacing(12).padding(12));
    });

    // Add new project
    std::string newProject = "";
    auto addProjectUI = HBox({
        InputField().placeholder("New project name...").on_change([&](auto val) {
            newProject = val;
        }).expand(),
        Button("Add Project").on_click([&]() {
            if (!newProject.empty()) {
                auto& projects = state["projects"].get<std::map<std::string, std::vector<std::string>>>();
                projects[newProject] = {};
                state["projects"] = projects;
                newProject = "";
            }
        })
    }).padding(12).spacing(10).background(Color(230, 230, 230));

    // Compose full app
    app.set_content(
        VBox({
            Text("📁 Chai Project Tracker").padding(12).bold().size(20),
            addProjectUI,
            projectList,
            Locator("new-task-form")
        }).expand().spacing(10)
    );

    return app.run();
}
