import os
import re
import urllib.request
import urllib.error
from concurrent.futures import ThreadPoolExecutor

POKEMON_151 = [
    (1, "Bulbasaur"), (2, "Ivysaur"), (3, "Venusaur"), (4, "Charmander"), (5, "Charmeleon"),
    (6, "Charizard"), (7, "Squirtle"), (8, "Wartortle"), (9, "Blastoise"), (10, "Caterpie"),
    (11, "Metapod"), (12, "Butterfree"), (13, "Weedle"), (14, "Kakuna"), (15, "Beedrill"),
    (16, "Pidgey"), (17, "Pidgeotto"), (18, "Pidgeot"), (19, "Rattata"), (20, "Raticate"),
    (21, "Spearow"), (22, "Fearow"), (23, "Ekans"), (24, "Arbok"), (25, "Pikachu"),
    (26, "Raichu"), (27, "Sandshrew"), (28, "Sandslash"), (29, "Nidoran-f"), (30, "Nidorina"),
    (31, "Nidoqueen"), (32, "Nidoran-m"), (33, "Nidorino"), (34, "Nidoking"), (35, "Clefairy"),
    (36, "Clefable"), (37, "Vulpix"), (38, "Ninetales"), (39, "Jigglypuff"), (40, "Wigglytuff"),
    (41, "Zubat"), (42, "Golbat"), (43, "Oddish"), (44, "Gloom"), (45, "Vileplume"),
    (46, "Paras"), (47, "Parasect"), (48, "Venonat"), (49, "Venomoth"), (50, "Diglett"),
    (51, "Dugtrio"), (52, "Meowth"), (53, "Persian"), (54, "Psyduck"), (55, "Golduck"),
    (56, "Mankey"), (57, "Primeape"), (58, "Growlithe"), (59, "Arcanine"), (60, "Poliwag"),
    (61, "Poliwhirl"), (62, "Poliwrath"), (63, "Abra"), (64, "Kadabra"), (65, "Alakazam"),
    (66, "Machop"), (67, "Machoke"), (68, "Machamp"), (69, "Bellsprout"), (70, "Weepinbell"),
    (71, "Victreebel"), (72, "Tentacool"), (73, "Tentacruel"), (74, "Geodude"), (75, "Graveler"),
    (76, "Golem"), (77, "Ponyta"), (78, "Rapidash"), (79, "Slowpoke"), (80, "Slowbro"),
    (81, "Magnemite"), (82, "Magneton"), (83, "Farfetch'd"), (84, "Doduo"), (85, "Dodrio"),
    (86, "Seel"), (87, "Dewgong"), (88, "Grimer"), (89, "Muk"), (90, "Shellder"),
    (91, "Cloyster"), (92, "Gastly"), (93, "Haunter"), (94, "Gengar"), (95, "Onix"),
    (96, "Drowzee"), (97, "Hypno"), (98, "Krabby"), (99, "Kingler"), (100, "Voltorb"),
    (101, "Electrode"), (102, "Exeggcute"), (103, "Exeggutor"), (104, "Cubone"), (105, "Marowak"),
    (106, "Hitmonlee"), (107, "Hitmonchan"), (108, "Lickitung"), (109, "Koffing"), (110, "Weezing"),
    (111, "Rhyhorn"), (112, "Rhydon"), (113, "Chansey"), (114, "Tangela"), (115, "Kangaskhan"),
    (116, "Horsea"), (117, "Seadra"), (118, "Goldeen"), (119, "Seaking"), (120, "Staryu"),
    (121, "Starmie"), (122, "Mr. Mime"), (123, "Scyther"), (124, "Jynx"), (125, "Electabuzz"),
    (126, "Magmar"), (127, "Pinsir"), (128, "Tauros"), (129, "Magikarp"), (130, "Gyarados"),
    (131, "Lapras"), (132, "Ditto"), (133, "Eevee"), (134, "Vaporeon"), (135, "Jolteon"),
    (136, "Flareon"), (137, "Porygon"), (138, "Omanyte"), (139, "Omastar"), (140, "Kabuto"),
    (141, "Kabutops"), (142, "Aerodactyl"), (143, "Snorlax"), (144, "Articuno"), (145, "Zapdos"),
    (146, "Moltres"), (147, "Dratini"), (148, "Dragonair"), (149, "Dragonite"), (150, "Mewtwo"),
    (151, "Mew")
]

OUT_DIR = "images"
os.makedirs(OUT_DIR, exist_ok=True)

def fetch_image(urls):
    headers = {"User-Agent": "Mozilla/5.0"}
    for url in urls:
        try:
            req = urllib.request.Request(url, headers=headers)
            with urllib.request.urlopen(req, timeout=10) as resp:
                if resp.status == 200:
                    data = resp.read()
                    if len(data) > 100:
                        return data
        except Exception:
            continue
    return None

def download_pokemon(poke_info):
    dex_id, name = poke_info
    
    # URL patterns for Gen 3 (firered-leafgreen, emerald, ruby-sapphire)
    front_urls = [
        f"https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/firered-leafgreen/{dex_id}.png",
        f"https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/emerald/{dex_id}.png",
        f"https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/ruby-sapphire/{dex_id}.png",
    ]
    back_urls = [
        f"https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/firered-leafgreen/back/{dex_id}.png",
        f"https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/emerald/back/{dex_id}.png",
        f"https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/ruby-sapphire/back/{dex_id}.png",
    ]

    clean_name = re.sub(r"[^a-zA-Z0-9]+", "_", name.lower()).strip("_")
    raw_lower = name.lower()

    # Front
    front_data = fetch_image(front_urls)
    if front_data:
        p1 = os.path.join(OUT_DIR, f"{clean_name}_front.png")
        with open(p1, "wb") as f:
            f.write(front_data)
        if clean_name != raw_lower:
            p2 = os.path.join(OUT_DIR, f"{raw_lower}_front.png")
            with open(p2, "wb") as f:
                f.write(front_data)

    # Back
    back_data = fetch_image(back_urls)
    if back_data:
        p1 = os.path.join(OUT_DIR, f"{clean_name}_back.png")
        with open(p1, "wb") as f:
            f.write(back_data)
        if clean_name != raw_lower:
            p2 = os.path.join(OUT_DIR, f"{raw_lower}_back.png")
            with open(p2, "wb") as f:
                f.write(back_data)

    return dex_id, name, front_data is not None, back_data is not None

def main():
    print(f"Starting download of Gen 3 sprites for 151 Pokemon into '{OUT_DIR}'...")
    success_count = 0
    with ThreadPoolExecutor(max_workers=12) as executor:
        for dex_id, name, f_ok, b_ok in executor.map(download_pokemon, POKEMON_151):
            status = f"Front: {'OK' if f_ok else 'FAIL'}, Back: {'OK' if b_ok else 'FAIL'}"
            if f_ok and b_ok:
                success_count += 1
            print(f"#{dex_id:03d} {name:<12}: {status}")
    print(f"\nDownload complete! Successfully downloaded sprites for {success_count}/151 Pokemon.")

if __name__ == "__main__":
    main()
