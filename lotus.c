#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define EXPECT(condition, ...) \
    do { \
        if (!(condition)) { \
            SDL_Log(__VA_ARGS__); \
            goto error; \
        } \
    }while (0)

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

#define WORD_MAX 255
#define TRY_MAX 20

#define FONT_SIZE SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE

static const SDL_Color RIGHT_COLOR   = {255, 0, 0, 255};
static const SDL_Color PRESENT_COLOR = {255, 150, 0, 255};
static const SDL_Color EMPTY_COLOR   = {0, 0, 0, 255};

#define NUM_WORDS 297
static const char* WORDS[NUM_WORDS] = {
    "ABAT", "ABIME", "ABOND", "ACCUEIL", "ACIDE", "ACTEUR", "ADIEU", "AGENT", "AGIR", "ALICE", 
    "AMOUR", "ANIMAL", "APPETIT", "ARBRE", "ARGENT", "ARMEE", "ARTISTE", "ASTUCE", "AUBE", "AUTRE", 
    "AVION", "BALLE", "BANQUE", "BARRE", "BASE", "BASQUET", "BESOIN", "BLANC", "BLESSER", "BOISSON", 
    "BOUCLE", "BOUTON", "BRAVO", "BULLE", "CAMPE", "CANAL", "CARRE", "CAVALIER", "CHANT", "CHAOS", 
    "CHAUFFAGE", "CHEVAL", "CHOSE", "CIGARETTE", "CINEMA", "CLINIQUE", "CLOCHER", "COEUR", "COLLE", 
    "CONSEIL", "CORPS", "COTON", "COURIR", "CRANE", "CRISE", "CROIX", "CUIR", "DANGER", "DART", 
    "DEBAT", "DEMAIN", "DESSIN", "DIALOGUE", "DIFFUSION", "DOUCEUR", "DROIT", "DURER", "ECRIRE", 
    "EFFECTIF", "EMPLOI", "ENFER", "ENVOI", "EQUIPE", "ESCAPE", "FABLE", "FAIRE", "FETE", "FLOTTANT", 
    "FONCTION", "FORCE", "FORUM", "FOUR", "FRIEND", "GENIE", "GROUPE", "GUERRE", "HOMME", "IMMEUBLE", 
    "INVENTER", "JOIE", "JOUEUR", "JOUR", "JUMELLE", "KIF", "LIVRE", "LUMIERE", "MAGIE", "MAIRE", 
    "MARCHER", "MASSAGE", "MATHS", "MEILLEUR", "MELANGE", "MINES", "MOBILE", "MONTAGNE", "MOTIF", 
    "MOUVEMENT", "MUR", "NATURE", "NEIGE", "NOUVEAU", "NUIT", "NUMERO", "OCCUPER", "ONZE", 
    "OPERATION", "ORANGE", "ORDRE", "OUTIL", "PAPIER", "PARIS", "PAYER", "PERDU", "PERSONNE", 
    "PIERRE", "PLAGE", "PLAISIR", "POINT", "POMME", "PORT", "PREMIER", "PRIX", "PRODUIRE", 
    "RANGE", "RECHERCHER", "REPARTIR", "REUSSIR", "RIDEAU", "ROUGE", "SALLE", "SAVOIR", "SILENCE", 
    "SOMME", "SON", "SORTIE", "SOUFFLE", "SOURIRE", "SOUVENIR", "SPECTACLE", "SPORT", "SUCRE", 
    "TACHE", "TABLE", "TACHE", "TANT", "TEXTE", "THEME", "TIGRE", "TONNERRE", "TOUR", "VAGUE", 
    "VAPEUR", "VENT", "VIE", "VIEUX", "VIRAGE", "VOLER", "VOIE", "VOIR", "VOL", "VOYAGE", 
    "ZEBRE", "ZONER", "ZEROS", "ZOUK", "ZEN", "SOURIS", "SOLIDE", "RAPIDE", "POLICE", "RIVIERA", 
    "LUXE", "BOND", "PANDAS", "ZOMBIE", "ANALYSE", "PORTIER", "MOULE", "RAMES", "MIRE", "RISQUE", 
    "TREVE", "NAGER", "HAUTEUR", "DECOUPER", "VIGIE", "MONDE", "POMMEAU", "REGLE", "FACILE", 
    "TROIS", "SOMBRE", "VIRTUE", "QUATRE", "CINQ", "AIMER", "FAMILLE", "MONTER", "NOUVEAU", 
    "MELON", "TISANE", "SALAIRE", "TEST", "BATEAU", "PHOQUE", "DERIVE", "MALIN", "GRAINE", 
    "ANNEAU", "AVENTURE", "LOUP", "REGAL", "CAVALIER", "BOL", "REVOIR", "COUTEAU", "PAROLE", 
    "BOLIDE", "GLOBE", "EXAMEN", "TOUS", "ORIGINE", "DRAME", "ACIER", "DOLCE", "PLANTER", 
    "BOL", "MENTIR", "FLOP", "SAVOIR", "CHIFFRE", "ECLAT", "TAUX", "VAMPIRE", "PORTE", 
    "LANCE", "DOIGT", "BATON", "PLIE", "ARGILE", "MER", "SOL", "FOU", "POUR", "METAL", 
    "RANGER", "TOURNER", "MOLLE", "VOITURE", "GENOU", "PRENOM", "PREMIER", "SQUELETTE", 
    "HIVER", "PELOTE", "POTERIE", "LIRE", "VALEUR", "HAUT", "ORDRE", "SOMME", "AMBRE", 
    "PRIER", "FOURNEAU", "FANATIQUE", "ZEBRA", "MUSE", "LOUE", "PENDRE", "CALME", 
    "DOSE", "NIVEAU", "PIPER", "SOMME", "RAFALE", "NUAGE", "MOULIN", "PAIRE", 
    "ANIMAL", "PAON", "VAGUE", "CRAVATE", "ROUGE", "TAPIS", "PANIQUE", "VOILE"
};


typedef struct Cell
{
    char letter;
    SDL_Color color;
}Cell;

typedef struct AppState
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    char word[WORD_MAX]; // ASCII
    
    int word_length;
    int num_tries;
    
    int current_try;
    int current_letter;
    
    Cell grid[TRY_MAX][WORD_MAX];
    Uint64 solved_time;
    
    SDL_Texture* grid_texture;
}AppState;

void SetWord(AppState* app)
{
    SDL_snprintf(app->word, WORD_MAX, "%s", WORDS[SDL_rand(NUM_WORDS)]);
}

void Init(AppState* app)
{
    SetWord(app);
    app->word_length = SDL_strlen(app->word);
    app->num_tries = 8;
    app->current_try = 0;
    app->current_letter = 0;
    app->solved_time = 0;
    
    for (int i = 0; i < app->num_tries; i++)
    {
        for (int j = 0; j < app->word_length; j++)
        {
            app->grid[i][j].color = EMPTY_COLOR;
            app->grid[i][j].letter = '\0';
        }
    }
    
    app->grid[0][0] = (Cell){app->word[0], RIGHT_COLOR};
    app->current_letter++;
    
    if (app->grid_texture) SDL_DestroyTexture(app->grid_texture);
    app->grid_texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, app->word_length * (FONT_SIZE + 2) - 1, app->num_tries * (FONT_SIZE + 2) - 1);
    SDL_SetTextureScaleMode(app->grid_texture, SDL_SCALEMODE_NEAREST);
    SDL_SetRenderLogicalPresentation(app->renderer, app->grid_texture->w, app->grid_texture->h, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

SDL_AppResult SDL_AppInit(void** userdata, int argc, char* argv[])
{
    AppState* app = SDL_calloc(1, sizeof(AppState));
    *userdata = app;
    
    EXPECT(SDL_Init(SDL_INIT_VIDEO), "%s", SDL_GetError());
    EXPECT(SDL_CreateWindowAndRenderer("Lotus", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &app->window, &app->renderer), "%s", SDL_GetError());
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");
    SDL_srand(0);
    SDL_StartTextInput(app->window);
    
    Init(app);

    return SDL_APP_CONTINUE;

error:
    return SDL_APP_FAILURE;
}

bool Solved(AppState* app, const Cell* solution)
{
    for (int i = 0; i < app->word_length; i++)
    {
        if (solution[i].letter != app->word[i])
            return false;
    }
    return true;
}

void Evaluate(AppState* app)
{
    int occurences[26] = {0};
    
    for (char* it = app->word; *it; it++)
    {
        occurences[(*it) - 'A']++;
    }
    
    for (int i = 0; i < app->word_length; i++)
    {
        char letter = app->grid[app->current_try][i].letter;
        int index = letter - 'A';
        
        if (occurences[index] > 0)
        {
            if (app->word[i] == letter)
                app->grid[app->current_try][i].color = RIGHT_COLOR;
            else
                app->grid[app->current_try][i].color = PRESENT_COLOR;
                
            occurences[index]--;
        }
    }

    app->current_try++;
    app->current_letter = 0;
    
    if (Solved(app, app->grid[app->current_try - 1]))
    {
        app->solved_time = SDL_GetTicks();
    }
}

SDL_AppResult SDL_AppEvent(void* userdata, SDL_Event* event)
{
    AppState* app = userdata;
    
    if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
    
    if (event->type == SDL_EVENT_TEXT_INPUT)
    {
        if (app->current_letter < app->word_length)
        {
            char letter = event->text.text[0];
            if ((letter >= 'A') && (letter <= 'z'))
            {
                if ((letter >= 'a') && (letter <= 'z'))
                    letter -= 'a' - 'A';
                    
                app->grid[app->current_try][app->current_letter].letter = letter;
                app->current_letter++;
            }
        }
    }
    else if ((event->type == SDL_EVENT_KEY_DOWN) && (!event->key.repeat))
    {
        if (event->key.scancode == SDL_SCANCODE_BACKSPACE)
        {
            if ((app->current_letter > 0) && ((app->current_try != 0) || (app->current_letter != 1)))
            {
                app->current_letter--;
                app->grid[app->current_try][app->current_letter].letter = '\0';
            }
        }
        else if (event->key.scancode == SDL_SCANCODE_RETURN)
        {
            if (app->current_letter == app->word_length)
            {
                Evaluate(app);
            }
        }
    }
    
    return SDL_APP_CONTINUE;
}

void SetRenderColor(SDL_Renderer* renderer, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Render(AppState* app)
{
    SDL_SetRenderTarget(app->renderer, app->grid_texture);
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(app->renderer, NULL);
    
    for (int i = 0; i < app->num_tries; i++)
    {
        for (int j = 0; j < app->word_length; j++)
        {
            SDL_FRect rect = {j * FONT_SIZE + (2 * j), i * FONT_SIZE + (2 * i), FONT_SIZE + 1, FONT_SIZE + 1};
            SetRenderColor(app->renderer, app->grid[i][j].color);
            SDL_RenderFillRect(app->renderer, &rect);
            
            SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
            SDL_RenderDebugText(app->renderer, j * FONT_SIZE + (2 * j + 1), i * FONT_SIZE + (2 * i + 1), (char[]){app->grid[i][j].letter, '\0'});
        }
    }
    
    SDL_SetRenderTarget(app->renderer, NULL);
    SDL_RenderTexture(app->renderer, app->grid_texture, NULL, NULL);

    if ((app->solved_time != 0) && (SDL_GetTicks() - app->solved_time > 1000))
        Init(app);
}

SDL_AppResult SDL_AppIterate(void* userdata)
{
    AppState* app = userdata;
    
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    
    Render(app);
    
    SDL_RenderPresent(app->renderer);
    
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* userdata, SDL_AppResult result)
{
    if (userdata) SDL_free(userdata);
}
