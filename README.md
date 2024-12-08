# Ανάπτυξη Λογισμικού για Πληροφοριακά Συστήματα [Κ23α]
Εύρεση Κ-Εγγύτερων Γειτόνων με τη χρήση ενός [ευρετηρίου Vamana](https://dl.acm.org/doi/pdf/10.5555/3454287.3455520).  
Επέκταση υλοποίησης για [χρήση φίλτρων](https://dl.acm.org/doi/pdf/10.1145/3543507.3583552).
Γλώσσα υλοποίησης: C++

## Μέλη Ομάδας
* Ανδρέας Γεωργίου | 1115 2021 00218
* Αλέξανδρος Θεοφυλάκτου | 1115 2021 00220
* Κωνσταντίνος Θεοφυλάκτου | 1115 2021 00221

## Οδηγίες Χρήσης
### System Dependencies:
* c++17 standard library
* ubuntu environment
### Build Instructions
Το αρχείο .cpp πρέπει να συμπεριλαμβάνει το header file ```include/interface.hpp```.  
Μέσω αυτού του αρχείου το .cpp file εχει πρόσβαση σε όλα τα header files εντός του include directory.  
Για την showcase περίπτωση του αρχείου main.cpp:
 * Δημιουργία Εκτελέσιμου: ```$make```.
 * Δημιουργία Εκτελέσιμου και Τρέξιμο (με προκαθορισμένες τιμές παραμέτρων): ```$make run_<v | f | s>``` (αναλόγως του είδους ευρετηριού που θέλουμε να δημιουργήσουμε).  
 * Εκκαθάριση: ```$make clean```.  
Με τη δημιουργία Εκτελέσιμου, δημιουργούνται αυτόματα τα directories ```build/``` και ```bin/``` στα οποία αποθηκεύονται τα object files (.o) και εκτελέσιμα αντίστοιχα.  
Με την εκκαθάριση, αυτά τα directories διαγράφονται.

### Run Instructions
To showcase εκτελέσιμο που δημιουργείται από το αρχείο ```src/main.cpp``` δέχεται πολλαπλά προαιρετικά ορίσματα, και τουλάχιστον ένα υποχρεωτικό.  
Το υποχρεωτικό όρισμα είναι το ```index type``` που παίρνει τιμές: ```--vamana``` ```--filtered``` ```--stitched```.
Σε περίπτωση που δεν δοθούν άλλα ορίσματα, τα προαιρετικά ορίσματα παίρνουν προκαθορισμένες τιμές.  

Τα προαιρετικά ορίσματα είναι:
* ```-k``` - αριθμός γειτόνων
* ```-L``` - μέγιστος πληθικός αριθμός του candidate set
* ```-R``` - βαθμός κορυφών του γράφου
* ```-a``` - greedy συντελεστής a (defaults to 1.0f)
* ```-n_threads``` - Ο αριθμός των threads που θα χρησιμοποιεί το σύστημα (defaults to 1 - serial)
* ```-t``` - το ποσοστό (0,1] από samples της κάθε κατηγορίας για την εύρεση του medoid της κατηγορίας αυτής (πχ. t = 0.5 κοιτάζει μόνο τα μισά σημεία από κάθε κατηγορία για την εύρεση του medoid της)
* ```-Ls``` - Lsmall για το stitched vamana index
* ```-Rs``` - Rsmall για το stitched vamana index
* ```-n_data``` - ο αριθμός των δεδομένων εντός του data file
* ```-n_queries``` - ο αριθμός των ερωτημάτων εντός του queries file
* ```-n_grountruths``` - ο αριθμός των groundtruths στο groundtruth file (παράλληλο με το queries file)
* ```-dim_data``` - η διάσταση των δεδομένων εντός του data file
* ```-dim_query``` - η διάσταση των ερωτημάτων εντός του queries file
* ```-store``` - το αρχείο αποθήκευσης του index. Αν δεν δοθεί τότε δεν γίνεται αποθήκευση.
* ```-load``` - το αρχείο φόρτωσης του index. Αν δεν δοθεί τότε δεν γίνεται φόρτωση από file.
* ```-data``` - το αρχείο δεδομένων
* ```-queries``` - το αρχείο ερωτημάτων
* ```-groundtruth``` - το αρχείο groundtruth
* ```--create``` - flag για τη δημιουργία νέου index
* ```--debug``` - flag για την εμφάνιση βοηθητικών μηνυμάτων

Τα ορίσματα μπορούν να δοθούν με οποιαδήποτε σειρά με τον εξής τρόπο: :
* Τα ορίσματα μονής παύλας: ```-<argument> <value>```
* Τα ορίσματα διπλής παύλας είναι flags. Αρκεί μόνο: ```--<argument>```


Οι default τιμές βρίσκονται ορισμένες στο ```parse_args``` εντός του ```config.hpp``` και μπορούν να αλλαχθούν εφόσον ο χρήστης επιθυμεί.


### Type Dependencies
Η συγκεκριμένη υλοποίηση είναι abstract και λειτουργεί για κάθε τύπο δεδομένων με τις παρακάτω προϋποθέσεις:
 * Υπάρχει overloaded το ```less operator <```
 * Υπάρχει overloaded το ```equality operator =```
 * Υπάρχει overloaded το ```ostream operator <<``` (προαιρετικό αν πρόκειται να γίνει χρήση της μεθόδου ```DirectedGraph::store```)
 * Υπάρχει overloaded το ```istream operator >>``` (προαιρετικό αν πρόκειται να γίνει χρήση της μεθόδου ```DirectedGraph::load```)
 * Υπάρχει ορισμένη συνάρτηση απόστασης για τον συγκεκριμένο τύπο (όρισμα στον γράφο)
 * Υπάρχει ορισμένη συνάρτηση ελέγχου εγκυρότητας για τον συγκεκριμένο τύπο (προαιρετικό όρισμα στον γράφο)

  
Για τα παραπάνω υπάρχουν ήδη υλοποιημένα παραδείγματα εντός του αρχείου ```include/config.hpp``` για τον αρκετά απλό τύπο δεδομένων ```vector<float>```, οπου μερικές υλοποιήσεις είναι πιο γενικές για τις δομές ```vector<T>``` όπου ```T``` είναι ένας τύπος δεδομένων που έχει τα πιο πάνω overloads.  

Σε περίπτωση που ο χρήστης θέλει να χρησιμοποιήσει τη συνάρτηση ```evaluateIndex``` εντός του ```interface.hpp```, πρέπει να έχει ορισμένη μία συνάρτηση η οποία διαβάζει τα queries από το specified file που έδωσε σαν όρισμα, και επιστρέφει ένα ```vector<Query<T>>``` με well-constructed queries.  

Υπάρχουν ήδη υλοποιημένες δύο συναρτήσεις, οι ```read_queries_vecs``` και ```read_queries_bin_contest``` που χρησιμοποιούνται αναλόγως του queries file format.

## Σχεδιαστικές Επιλογές:
Η υλοποίηση του [Vamana Index](https://dl.acm.org/doi/pdf/10.5555/3454287.3455520) και του [Filtered Vamana Index (Filtered or Stitched approach)](https://dl.acm.org/doi/pdf/10.1145/3543507.3583552) γίνεται μέσω της κλάσης DirectedGraph.

Στο αρχείο ```src/main.cpp``` δίνεται ένα παράδειγμα εκτέλεσης, και το πως χρησιμοποιούνται οι απαραίτητες μέθοδοι της κλάσης. Εσωτερικά η κλάση DirectedGraph χρησιμοποιεί και δικές τις μεθόδους οι οποίες είναι accessible και από τον χρήστη. Οι μέθοδοι και τα πεδία που είναι marked με underscore (π.χ. ```_parallel_medoid```) χρησιμοποιούνται μόνο για εσωτερική χρήση και δεν είναι accessible από τον χρήστη.  

Η υλοποίηση της κλάσης του γράφου είναι ένα template που δέχεται οποιοδήποτε τύπο δεδομένων που πληροί τις [παραπάνω προϋποθέσεις](#type-dependencies).  
Για το ορθό instantiation ενός αντικειμένου τύπου DirectedGraph απαιτούνται:
* Συνάρτηση <b>μετρικής</b> απόστασης: $T \times T\to \mathbb{R}^+_0$
* Συνάρτηση εγκυρότητας: $T\to$ { $true, false$ }  που επιστρέφει $true$ αν ένα συγκεκριμένο instance δεν είναι έγκυρο π.χ. ```isEmpty(nullptr)``` επιστρέφει $true$    
όπου $T$ είναι ο χώρος των αντικειμένων του τύπου δεδομένων.


Χρησιμοποιεί την προσέγγιση λιστών γειτνίασης μέσω της δομής ```std::unordered_map``` για να αποθηκεύσει τις ακμές στον γράφο. Μία ακμή είναι ένα ζευγάρι από Ids, οπου τα Ids είναι ένας τύπος δεδομένων που μιμείται το int, με default τιμή -1.  
Τα δεδομένα αποθηκεύονται σε μία δομή ```std::vector<Node<T>>``` και γίνονται encapsulated σε μία κλάση Node. Η Κλάση Node κρατάει το Id του συγκεκριμένου node, το value, και την κατηγορία στην οποία ανήκει (αν ανήκει, αλλιώς -1).

### Παραδοχές:
Στην εύρεση των medoids για συγκεκριμένες κατηγορίες, χρησιμοποιείται η παράμετρος ```threshold```. Στο [paper](https://dl.acm.org/doi/pdf/10.1145/3543507.3583552) αναφέρεται ότι το ```threshold``` είναι ένας σταθερός αριθμός. Παρόλα αυτά, υπάρχουν κατηγορίες με σημαντικά μικρότερο representation στο dataset, οπότε αποφασίσαμε πως το ```threshold``` θα είναι μία πολλαπλασιαστική παράμετρος ποσοστού που θα καθορίζει το ποσοστό των σημείων της κάθε κατηγορίας που θα ληφθούν υπόψη κατά τη διαδικασία εύρεσης του medoid της κατηγορίας αυτής.  

Η Υλοποίηση έγινε για δεδομένα που ανήκουν το πολύ σε μία κατηγορία. Συνεπώς πολλές πράξεις συνόλων απλοποιούνται σε ισότητες, ανισότητες και προσθέσεις (insertions).


## Αποτελέσματα Εκτέλεσης
**Vamana:** (dataset 1ου παραδοτέου, 10 000 points, 100 queries)  
* Χρόνος δημιουργίας ευρετηρίου: 00h:00m:41s  
* Χρόνος εκτέλεσης ερωτημάτων: 00h:00m:00s  
* Average recall score: 0.9759  
  
  
**Filtered-Vamana:** (dataset 2ου παραδοτέου, 1 000 000 points, 10 000 queries)
* Χρόνος δημιουργίας ευρετηρίου: 01h:24m:13s  
* Χρόνος εκτέλεσης ερωτημάτων: 00h:04m:29s  
* Average recall score: 0.959263 
  
**Stitched-Vamana:** (dataset 2ου παραδοτέου, 1 000 000 points, 10 000 queries)
* Χρόνος δημιουργίας ευρετηρίου: 02h:08m:03s  
* Χρόνος εκτέλεσης ερωτημάτων: 00h:04m:15s  
* Average recall score: 0.986903  


## Συμβολή Μελών Ομάδας
* Ανδρέας Γεωργίου | 1115 2021 00218 - Greedy Search, tests util, tests config, test types, store-load, config.hpp, graph template fixes, συναρτήσεις util fixes, Robust Prune fixes, Makefile additions, Node and Query, Filtered Greedy Search, Filtrered Vamana, Stitched Vamana, Graph Updates to handle filtered data and use Ids, Id class and overloads, Test updates to handle new structure (Ids, nodes, queries), interface functions implementation, cleanup, groundtruth generation, CLI flexibility, Documentation
* Αλέξανδρος Θεοφυλάκτου | 1115 2021 00220 - Makefile, test structure, test types, test util, Greedy Search, Robust Prune, Vamana Algorithm, store-load optimization, Συναρτήσεις util, graph template structure, main.cpp, config.hpp, evaluation.hpp, Node and Query, Filtered Vamana, Filtered Greedy Search, Filtered Robust Prune, Filtered Medoids, interface functions, read functions for files, groundtruth generation, store load updates, timing functions and evaluation, unit tests (filtered), CLI flexibility Documentation
* Κωνσταντίνος Θεοφυλάκτου | 1115 2021 00221 - Makefile, Συναρτήσεις util, serial + parallel medoid, greedy search, Rgraph, Robust Prune, Vamana Algorithm, store-load, config.hpp, test util, graph template structure, c_log, interface.hpp, Node and Query, Overloads, Filtered Vamana, Filtered Greedy Search, Filtered Robust Prune, Filtered Medoids, Stitched Vamana, Graph Updates to handle filtered data and updates to use Ids and overloads, Interface function fixes, optimizations, init_test, CLI types and formats flexibility (user) Documentation
