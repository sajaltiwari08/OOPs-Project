
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <queue>
#include <stdexcept>
#include <cstdio>

using namespace std;

// ============================================================
// UNIT I: NAMESPACES, INLINE FUNCTIONS, FUNCTION OVERLOADING
// ============================================================

namespace ReliefOps {

namespace Logistics {

inline double calculateDistance(double x1, double y1,
                               double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

inline double calculatePayloadCapacity(double weight, double capacity) {
    return (capacity > 0) ? (weight / capacity) * 100.0 : 0.0;
}

class IncidentCommander;

class ReliefCargo {
private:
    string name;
    int quantity;
    double weightPerUnit;
    int priority;

public:
    ReliefCargo(string n = "Unknown", int q = 0,
               double w = 1.0, int p = 1)
        : name(n), quantity(q), weightPerUnit(w), priority(p) {}

    string getName() const { return name; }
    int getQuantity() const { return quantity; }
    double getWeight() const { return quantity * weightPerUnit; }
    double getUnitWeight() const { return weightPerUnit; }
    int getPriority() const { return priority; }

    void setQuantity(int q) { quantity = q; }

    // Operator == : check cargo identity
    bool operator==(const ReliefCargo& c) const {
        return name == c.name;
    }

    // Operator += : restock cargo
    ReliefCargo& operator+=(int q) {
        quantity += q;
        return *this;
    }

    // Operator -= : deduct cargo
    ReliefCargo& operator-=(int q) {
        if (q > quantity)
            throw runtime_error("Insufficient cargo quantity!");
        quantity -= q;
        return *this;
    }

    // Stream insertion
    friend ostream& operator<<(ostream& out, const ReliefCargo& c) {
        out << left << setw(22) << c.name
            << setw(10) << c.quantity
            << setw(12) << c.weightPerUnit
            << setw(10) << c.priority;
        return out;
    }

    // Stream extraction
    friend istream& operator>>(istream& in, ReliefCargo& c) {
        cout << "Enter cargo name, quantity, unit weight, priority: ";
        in >> ws;
        getline(in, c.name);
        in >> c.quantity >> c.weightPerUnit >> c.priority;
        return in;
    }

    friend class IncidentCommander;
};

// Custom exceptions
class CargoOverweightException : public exception {
public:
    const char* what() const noexcept override {
        return "Cargo exceeds vehicle payload capacity!";
    }
};

class FuelDepletionException : public exception {
public:
    const char* what() const noexcept override {
        return "Vehicle fuel is insufficient for the journey!";
    }
};

class DamagedCorridorException : public exception {
public:
    const char* what() const noexcept override {
        return "Route is damaged or blocked!";
    }
};

// Friend class for emergency inventory override
class IncidentCommander {
public:
    void emergencyOverride(ReliefCargo& c, int q) {
        cout << "\n[COMMANDER OVERRIDE] Emergency supply allocation!\n";
        c.quantity = q; // Access private data
    }
};

// Function overloading for supply allocation
void allocateSupply(ReliefCargo& c, int quantity) {
    c += quantity;
}

void allocateSupply(ReliefCargo& c, int quantity, int emergencyPriority) {
    c += quantity;
    cout << "Emergency priority " << emergencyPriority
         << " allocation approved.\n";
}

void allocateSupply(ReliefCargo& c, double volume, double density) {
    int quantity = static_cast<int>(volume * density);
    c += quantity;
}

// ============================================================
// UNIT II: ENCAPSULATION, DYNAMIC MEMORY, OPERATOR OVERLOADING
// ============================================================

class DistributionDepot {
private:
    string depotName;
    unordered_map<string, ReliefCargo> inventory;

public:
    DistributionDepot(string n) : depotName(n) {}

    string getName() const { return depotName; }

    void addCargo(const ReliefCargo& c) {
        auto it = inventory.find(c.getName());

        if (it == inventory.end())
            inventory.emplace(c.getName(), c);
        else
            it->second += c.getQuantity();
    }

    ReliefCargo& getCargo(string name) {
        auto it = inventory.find(name);
        if (it == inventory.end())
            throw runtime_error("Cargo not available in depot!");
        return it->second; // Return by reference
    }

    bool hasCargo(string name, int quantity) const {
        auto it = inventory.find(name);
        return it != inventory.end() && it->second.getQuantity() >= quantity;
    }

    void displayInventory() const {
        cout << "\n========== " << depotName << " INVENTORY ==========\n";
        cout << left << setw(22) << "Cargo"
             << setw(10) << "Quantity"
             << setw(12) << "Unit(Kg)"
             << setw(10) << "Priority" << "\n";
        cout << string(54, '-') << "\n";

        for (const auto& entry : inventory)
            cout << entry.second << "\n";
    }
};

// GPS composition
class GPSBeacon {
private:
    double x, y;

public:
    GPSBeacon(double a = 0, double b = 0) : x(a), y(b) {}

    void update(double a, double b) {
        x = a;
        y = b;
    }

    double distanceTo(double a, double b) const {
        return calculateDistance(x, y, a, b);
    }

    void display() const {
        cout << "GPS Coordinates: (" << x << ", " << y << ")\n";
    }
};

// Emergency life support composition
class EmergencyLifeSupportUnit {
private:
    bool active;

public:
    EmergencyLifeSupportUnit(bool a = true) : active(a) {}

    void activate() { active = true; }
    void deactivate() { active = false; }

    bool isActive() const { return active; }
};

// Static analytics
class TransportVehicle {
private:
    static int operationalVehicles;
    static double totalDispatchedTonnage;

protected:
    string vehicleID;
    double capacity;
    double fuel;
    ReliefCargo* manifest; // Dynamic memory
    int manifestCount;

    GPSBeacon gps; // Containership
    EmergencyLifeSupportUnit lifeSupport; // Containership

    void clearManifest() {
        delete[] manifest;
        manifest = nullptr;
        manifestCount = 0;
    }

public:
    TransportVehicle(string id, double cap, double f,
                    double x = 0, double y = 0)
        : vehicleID(id), capacity(cap), fuel(f),
          manifest(nullptr), manifestCount(0),
          gps(x, y), lifeSupport(true) {
        operationalVehicles++;
    }

    // Dynamic constructor
    TransportVehicle(string id, double cap, double f,
                    const ReliefCargo* items, int count)
        : vehicleID(id), capacity(cap), fuel(f),
          manifest(nullptr), manifestCount(count),
          gps(0, 0), lifeSupport(true) {
        operationalVehicles++;
        if (count > 0) {
            manifest = new ReliefCargo[count];
            for (int i = 0; i < count; i++)
                manifest[i] = items[i];
        }
    }

    // Deep copy constructor
    TransportVehicle(const TransportVehicle& other)
        : vehicleID(other.vehicleID), capacity(other.capacity),
          fuel(other.fuel), manifest(nullptr),
          manifestCount(other.manifestCount), gps(0, 0),
          lifeSupport(true) {
        operationalVehicles++;
        if (manifestCount > 0) {
            manifest = new ReliefCargo[manifestCount];
            for (int i = 0; i < manifestCount; i++)
                manifest[i] = other.manifest[i];
        }
    }

    // Deep copy assignment
    TransportVehicle& operator=(const TransportVehicle& other) {
        if (this != &other) {
            ReliefCargo* temp = nullptr;
            if (other.manifestCount > 0) {
                temp = new ReliefCargo[other.manifestCount];
                for (int i = 0; i < other.manifestCount; i++)
                    temp[i] = other.manifest[i];
            }

            clearManifest();
            vehicleID = other.vehicleID;
            capacity = other.capacity;
            fuel = other.fuel;
            manifestCount = other.manifestCount;
            manifest = temp;
        }
        return *this;
    }

    virtual ~TransportVehicle() {
        clearManifest();
        operationalVehicles--;
    }

    // Add cargo dynamically to manifest
    void addToManifest(const ReliefCargo& c) {
        ReliefCargo* temp = new ReliefCargo[manifestCount + 1];

        for (int i = 0; i < manifestCount; i++)
            temp[i] = manifest[i];

        temp[manifestCount] = c;

        delete[] manifest;
        manifest = temp;
        manifestCount++;
    }

    double currentLoad() const {
        double total = 0;
        for (int i = 0; i < manifestCount; i++)
            total += manifest[i].getWeight();
        return total;
    }

    void showManifest() const {
        cout << "\nManifest of " << vehicleID << ":\n";
        for (int i = 0; i < manifestCount; i++)
            cout << manifest[i] << "\n";
        cout << "Total load: " << currentLoad() << " Kg\n";
    }

    // Overloaded allocation: payload-based
    void loadCargo(const ReliefCargo& c) {
        if (currentLoad() + c.getWeight() > capacity)
            throw CargoOverweightException();
        addToManifest(c);
    }

    // Overloaded allocation: emergency medical triage
    void loadCargo(const ReliefCargo& c, int medicalPriority) {
        if (currentLoad() + c.getWeight() > capacity)
            throw CargoOverweightException();

        cout << "Medical triage priority: " << medicalPriority << "\n";
        addToManifest(c);
    }

    // Virtual route evaluation
    virtual void evaluateRoute(double x, double y) const {
        double distance = gps.distanceTo(x, y);
        if (fuel <= distance * 0.5)
            throw FuelDepletionException();

        cout << "Route distance: " << fixed << setprecision(2)
             << distance << " km\n";
    }

    virtual string getType() const {
        return "Transport Vehicle";
    }

    string getID() const { return vehicleID; }
    double getCapacity() const { return capacity; }

    void setGPS(double x, double y) {
        gps.update(x, y);
    }

    static int getOperationalVehicles() {
        return operationalVehicles;
    }

    static double getTotalTonnage() {
        return totalDispatchedTonnage;
    }

    static void recordDispatch(double kg) {
        totalDispatchedTonnage += kg / 1000.0;
    }
};

int TransportVehicle::operationalVehicles = 0;
double TransportVehicle::totalDispatchedTonnage = 0.0;

// ============================================================
// UNIT III: INHERITANCE, VIRTUAL BASE CLASSES, POLYMORPHISM
// ============================================================

// Virtual root class for diamond inheritance
class ReliefVehicle : public virtual TransportVehicle {
public:
    ReliefVehicle(string id, double cap, double f)
        : TransportVehicle(id, cap, f) {}

    virtual void evaluateRoute(double x, double y) const override {
        TransportVehicle::evaluateRoute(x, y);
    }

    virtual ~ReliefVehicle() {}
};

// Ground transport
class GroundConvoy : public virtual ReliefVehicle {
public:
    GroundConvoy(string id, double cap, double f)
        : TransportVehicle(id, cap, f), ReliefVehicle(id, cap, f) {}

    string getType() const override {
        return "Ground Convoy";
    }

    void evaluateRoute(double x, double y) const override {
        cout << "[Ground Route] ";
        ReliefVehicle::evaluateRoute(x, y);
    }
};

// Aerial transport
class AerialTransport : public virtual ReliefVehicle {
public:
    AerialTransport(string id, double cap, double f)
        : TransportVehicle(id, cap, f), ReliefVehicle(id, cap, f) {}

    string getType() const override {
        return "Aerial Transport";
    }

    void evaluateRoute(double x, double y) const override {
        cout << "[Air Route] ";
        ReliefVehicle::evaluateRoute(x, y);
    }
};

// Amphibious craft
class AmphibiousCraft : public virtual ReliefVehicle {
public:
    AmphibiousCraft(string id, double cap, double f)
        : TransportVehicle(id, cap, f), ReliefVehicle(id, cap, f) {}

    string getType() const override {
        return "Amphibious Craft";
    }

    void evaluateRoute(double x, double y) const override {
        cout << "[Water Route] ";
        ReliefVehicle::evaluateRoute(x, y);
    }
};

// Hybrid vehicle: diamond inheritance resolved using virtual base
class HybridMedicalUnit : public GroundConvoy, public AerialTransport {
public:
    HybridMedicalUnit(string id, double cap, double f)
        : TransportVehicle(id, cap, f),
          ReliefVehicle(id, cap, f),
          GroundConvoy(id, cap, f),
          AerialTransport(id, cap, f) {}

    string getType() const override {
        return "Hybrid Medical Unit";
    }

    void evaluateRoute(double x, double y) const override {
        cout << "[Hybrid Route] ";
        ReliefVehicle::evaluateRoute(x, y);
    }
};

// ============================================================
// UNIT IV: FILE HANDLING, STL, TEMPLATE, FUNCTOR, MANIPULATOR
// ============================================================

// Custom priority dispatch queue
template <typename T>
class PriorityDispatchQueue {
private:
    vector<T> items;

public:
    void push(const T& item) {
        items.push_back(item);
    }

    bool empty() const {
        return items.empty();
    }

    T pop() {
        if (empty())
            throw runtime_error("Dispatch queue is empty!");

        sort(items.begin(), items.end());
        T top = items.back();
        items.pop_back();
        return top;
    }

    void display() const {
        cout << "\nPriority Dispatch Queue:\n";
        vector<T> temp = items;
        sort(temp.begin(), temp.end());

        for (const auto& item : temp)
            cout << item << "\n";
    }
};

// Functor to sort by priority
struct UrgentCargoFirst {
    bool operator()(const ReliefCargo& a, const ReliefCargo& b) const {
        return a.getPriority() > b.getPriority();
    }
};

// User-defined output manipulator
ostream& manifestTable(ostream& out) {
    out << "\n========== RELIEF MANIFEST ==========\n";
    out << left << setw(22) << "Cargo"
        << setw(10) << "Quantity"
        << setw(12) << "Unit(Kg)"
        << setw(10) << "Priority" << "\n";
    out << string(54, '-') << "\n";
    return out;
}

// Priority queue needs sortable cargo
bool operator<(const ReliefCargo& a, const ReliefCargo& b) {
    return a.getPriority() < b.getPriority();
}

// Binary incident log record
struct IncidentRecord {
    char vehicleID[30];
    char cargoName[50];
    int quantity;
    double weight;
    double distance;
};

// Binary persistent logging with rollback
void saveIncident(const string& filename, const IncidentRecord& rec) {
    fstream file(filename, ios::binary | ios::in | ios::out);

    if (!file) {
        file.open(filename, ios::binary | ios::out);
        file.close();
        file.open(filename, ios::binary | ios::in | ios::out);
    }

    file.seekg(0, ios::end);
    streampos oldSize = file.tellg();

    file.seekp(oldSize);
    file.write(reinterpret_cast<const char*>(&rec), sizeof(rec));
    file.flush();

    if (!file) {
        file.clear();
        file.seekp(oldSize);
        file.close();
        throw runtime_error("Incident log failed!");
    }

    file.close();
}

// Read binary logs
void readIncidents(const string& filename) {
    ifstream file(filename, ios::binary);
    IncidentRecord rec;

    cout << "\n===== SAVED INCIDENT LOGS =====\n";

    while (file.read(reinterpret_cast<char*>(&rec), sizeof(rec))) {
        cout << "Vehicle: " << rec.vehicleID
             << " | Cargo: " << rec.cargoName
             << " | Quantity: " << rec.quantity
             << " | Weight: " << rec.weight << " Kg"
             << " | Distance: " << rec.distance << " Km\n";
    }
}

} // namespace Logistics

namespace Inventory {
    // Inventory-related namespace
    void heading() {
        cout << "\n--- INVENTORY MANAGEMENT ---\n";
    }
}

} // namespace ReliefOps

// ============================================================
// MAIN: MENU-DRIVEN ERL-HUB SYSTEM
// ============================================================

using namespace ReliefOps::Logistics;

int main() {
    DistributionDepot depot("Central Relief Depot");

    depot.addCargo(ReliefCargo("Ration Kits", 500, 2.0, 3));
    depot.addCargo(ReliefCargo("Medical Kits", 100, 5.0, 5));
    depot.addCargo(ReliefCargo("Water Tanks", 200, 10.0, 2));
    depot.addCargo(ReliefCargo("Blankets", 300, 1.5, 4));

    vector<ReliefVehicle*> fleet;

    fleet.push_back(new GroundConvoy("TRUCK-01", 5000, 300));
    fleet.push_back(new AerialTransport("HELI-01", 1500, 250));
    fleet.push_back(new AmphibiousCraft("BOAT-01", 3000, 200));
    fleet.push_back(new HybridMedicalUnit("HYBRID-01", 2000, 350));

    PriorityDispatchQueue<ReliefCargo> dispatchQueue;

    int choice;

    do {
        cout << "\n====================================\n";
        cout << " POST-EARTHQUAKE RELIEF LOGISTICS HUB\n";
        cout << "====================================\n";
        cout << "1. View depot inventory\n";
        cout << "2. Restock supplies\n";
        cout << "3. Dispatch relief vehicle\n";
        cout << "4. View vehicle fleet\n";
        cout << "5. View priority dispatch queue\n";
        cout << "6. View saved incident logs\n";
        cout << "7. Emergency commander override\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        try {
            if (choice == 1) {
                depot.displayInventory();
            }

            else if (choice == 2) {
                ReliefOps::Inventory::heading();

                string name;
                int quantity;

                cout << "Enter supply name (Ration Kits/Medical Kits/"
                        "Water Tanks/Blankets): ";
                cin.ignore();
                getline(cin, name);

                cout << "Enter quantity to restock: ";
                cin >> quantity;

                allocateSupply(depot.getCargo(name), quantity);
                cout << "Restocking successful!\n";
            }

            else if (choice == 3) {
                depot.displayInventory();

                string cargoName;
                int quantity, vehicleChoice;

                cout << "\nEnter cargo name: ";
                cin.ignore();
                getline(cin, cargoName);

                cout << "Enter quantity: ";
                cin >> quantity;

                ReliefCargo& stock = depot.getCargo(cargoName);

                if (quantity <= 0)
                    throw runtime_error("Quantity must be positive!");

                if (!depot.hasCargo(cargoName, quantity))
                    throw runtime_error("Insufficient depot stock!");

                cout << "\nSelect Vehicle:\n";
                for (size_t i = 0; i < fleet.size(); i++)
                    cout << i + 1 << ". " << fleet[i]->getType()
                         << " (" << fleet[i]->getID() << ")\n";

                cout << "Choice: ";
                cin >> vehicleChoice;

                if (vehicleChoice < 1 || vehicleChoice > (int)fleet.size())
                    throw runtime_error("Invalid vehicle choice!");

                ReliefVehicle* vehicle = fleet[vehicleChoice - 1];

                double destinationX, destinationY;

                cout << "Enter destination coordinates (X Y): ";
                cin >> destinationX >> destinationY;

                // Route evaluation uses runtime polymorphism
                vehicle->evaluateRoute(destinationX, destinationY);

                ReliefCargo cargo(cargoName, quantity,
                    stock.getUnitWeight(), stock.getPriority());

                vehicle->loadCargo(cargo);

                // Deduct inventory using operator -=
                stock -= quantity;

                cout << "\nDISPATCH SUCCESSFUL!\n";
                vehicle->showManifest();

                TransportVehicle::recordDispatch(cargo.getWeight());

                // Binary incident record
                IncidentRecord rec{};
                strncpy(rec.vehicleID, vehicle->getID().c_str(),
                        sizeof(rec.vehicleID) - 1);
                strncpy(rec.cargoName, cargoName.c_str(),
                        sizeof(rec.cargoName) - 1);
                rec.quantity = quantity;
                rec.weight = cargo.getWeight();
                rec.distance = calculateDistance(0, 0, destinationX, destinationY);

                saveIncident("incident_log.dat", rec);

                dispatchQueue.push(cargo);
            }

            else if (choice == 4) {
                cout << "\n===== VEHICLE FLEET =====\n";
                for (auto v : fleet)
                    cout << v->getID() << " | " << v->getType()
                         << " | Capacity: " << v->getCapacity() << " Kg\n";

                cout << "Operational vehicles: "
                     << TransportVehicle::getOperationalVehicles() << "\n";
                cout << "Total dispatched tonnage: "
                     << TransportVehicle::getTotalTonnage() << " tonnes\n";
            }

            else if (choice == 5) {
                dispatchQueue.display();
            }

            else if (choice == 6) {
                readIncidents("incident_log.dat");
            }

            else if (choice == 7) {
                string name;
                int quantity;

                cout << "Enter cargo name: ";
                cin.ignore();
                getline(cin, name);

                cout << "Enter new emergency stock quantity: ";
                cin >> quantity;

                IncidentCommander commander;
                commander.emergencyOverride(depot.getCargo(name), quantity);
            }

            else if (choice == 0) {
                cout << "Exiting ERL-HUB. Stay safe!\n";
            }

            else {
                cout << "Invalid option!\n";
            }
        }
        catch (const exception& e) {
            cout << "\nERROR: " << e.what() << "\n";
        }

    } while (choice != 0);

    // Correct polymorphic destruction
    for (auto v : fleet)
        delete v;

    return 0;
}
